//============================================================================
// Copyright 2015 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "VModelData.hpp"

#include "AbstractNodeModel.hpp"
#include "NodeQuery.hpp"
#include "VFilter.hpp"
#include "ServerHandler.hpp"
#include "UserMessage.hpp"
#include "VAttribute.hpp"
#include "VNode.hpp"
#include "VTree.hpp"

#include <QDebug>
#include <QMetaMethod>

#define _UI_VMODELDATA_DEBUG

void VTreeChangeInfo::addStateChange(const VNode* n)
{
    VNode* s=n->suite();
    assert(s);
    if(std::find(stateSuites_.begin(),stateSuites_.end(),s) == stateSuites_.end())
        stateSuites_.push_back(s);
}

//==========================================
//
// VModelServer
//
//==========================================

//It takes ownership of the filter

VModelServer::VModelServer(ServerHandler *server) :
   server_(server),
   filter_(0),
   inScan_(0)
{
	//We has to observe the nodes of the server.
	server_->addNodeObserver(this);

    //We has to observe the server.
	server_->addServerObserver(this);

}

VModelServer::~VModelServer()
{
	server_->removeNodeObserver(this);
	server_->removeServerObserver(this);

	if(filter_)
		delete filter_;
}

int VModelServer::totalNodeNum() const
{
	return server_->vRoot()->totalNum();
}

void VModelServer::runFilter()
{
	if(filter_)
	{
        //filter_->beginReset(server_);
        //filter_->endReset();
	}
}

//==========================================
//
// VTreeServer
//
//==========================================

//It takes ownership of the filter

VTreeServer::VTreeServer(ServerHandler *server,NodeFilterDef* filterDef,AttributeFilter* attrFilter) :
   VModelServer(server),
   changeInfo_(new VTreeChangeInfo()),
   attrFilter_(attrFilter)
{
    tree_=new VTree(this);
	filter_=new TreeNodeFilter(filterDef);
	//We has to observe the nodes of the server.
	//server_->addNodeObserver(this);
}

VTreeServer::~VTreeServer()
{
    delete tree_;
    delete changeInfo_;
}

//--------------------------------------------------
// ServerObserver methods
//--------------------------------------------------

void VTreeServer::notifyDefsChanged(ServerHandler* server, const std::vector<ecf::Aspect::Type>& a)
{
    //When the defs changed we need to update the server node in the model/view
    Q_EMIT dataChanged(this);
    //TODO: what about node aor attr num changes!
}

void VTreeServer::notifyBeginServerScan(ServerHandler* server,const VServerChange& change)
{
    //When the server scan begins we must be in inScan mode so that the model should think that
    //this server tree is empty.
    inScan_=true;
    Q_ASSERT(tree_->numOfChildren() == 0);
    //Q_EMIT beginServerScan(this,tree_->numOfChildren() + tree_->attrNum(attrFilter_));
}

void VTreeServer::notifyEndServerScan(ServerHandler* /*server*/)
{
    //We still must be in inScan mode so that the model should think
    //that this server tree is empty.
    inScan_=true;

    Q_ASSERT(filter_);

    //When the server scan ends we need to rebuild the tree.
    if(filter_->isNull())
    {
        tree_->build();
    }
    else
    {
        filter_->update(server_);
        tree_->build(filter_->nodes_);
    }

    //Notifies the model of the number of children nodes to be added to the server node.
    Q_EMIT beginServerScan(this, tree_->attrNum(attrFilter_)+tree_->numOfChildren());
    //We leave the inScan mode. From this moment the model can see the whole tree in the server.
    inScan_=false;
    //Notifies the model that the scan finished. The model can now relayout its new contents.
    Q_EMIT endServerScan(this, tree_->attrNum(attrFilter_)+tree_->numOfChildren());
}

void VTreeServer::notifyBeginServerClear(ServerHandler* server)
{
    Q_EMIT beginServerClear(this,-1);
    tree_->clear();
    filter_->clear();
    inScan_=true;
}

void VTreeServer::notifyEndServerClear(ServerHandler* server)
{
	Q_EMIT endServerClear(this,-1);
}

void VTreeServer::notifyServerConnectState(ServerHandler* server)
{
	Q_EMIT rerender();
}

void VTreeServer::notifyServerActivityChanged(ServerHandler* server)
{
	Q_EMIT dataChanged(this);
}

//This is called when a normal sync (neither reset nor rescan) is finished. We have delayed the update of the
//filter to this point but now we need to do it.
void VTreeServer::notifyEndServerSync(ServerHandler* server)
{
#ifdef _UI_VMODELDATA_DEBUG
        UserMessage::debug("VTreeServer::notifyEndServerSync --> number of state changes: " + QString::number(changeInfo_->stateChangeSuites().size()).toStdString());
#endif

    //if there was a state change during the sync
    if(changeInfo_->stateChangeSuites().size() > 0 && !filter_->isNull())
	{
#ifdef _UI_VMODELDATA_DEBUG
        UserMessage::debug("  Suites changed:");
        for(size_t i= 0; i < changeInfo_->stateChangeSuites().size(); i++)
            UserMessage::debug("     " +  changeInfo_->stateChangeSuites().at(i)->strName());
#endif
        //Update the filter for the suites with a status change. topFilterChange
        //will contain the branches where the filter changed. A branch is
        //defined by the parent of the top level nodes with a filter status change in a given
        //suite. Suites are always visible (part of the tree) so a branch cannot be a server (root)
        //but at most a suite.
        std::vector<VNode*> topFilterChange;
        TreeNodeFilter *tf=static_cast<TreeNodeFilter*>(filter_);
        tf->update(server_,changeInfo_->stateChangeSuites(),topFilterChange);

#ifdef _UI_VMODELDATA_DEBUG
        UserMessage::debug("  Top level nodes that changed in filter:");
        for(size_t i= 0; i < topFilterChange.size(); i++)
            UserMessage::debug("     " +  topFilterChange.at(i)->strName());
#endif

        //A topFilterChange branch cannot be the root (server)
        for(size_t i=0; i < topFilterChange.size(); i++)
        {
            Q_ASSERT(!topFilterChange[i]->isServer());
        }

        //If something changed in the list of filtered nodes
        for(size_t i=0; i < topFilterChange.size(); i++)
        {
#ifdef _UI_VMODELDATA_DEBUG
            UserMessage::debug("  Branch: " + topFilterChange[i]->strName());
#endif
            //This is the branch where there is a change in the filter
            VTreeNode* tn=tree_->find(topFilterChange[i]);

            //If the branch is not in tree (not yet filtered) we need to
            //find it nearest ancestor up in the tree. This must exsit because
            //the suites are always part of the tree.
            if(!tn)
            {
                tn=tree_->findAncestor(topFilterChange[i]);
                Q_ASSERT(tn);
                Q_ASSERT(!tn->isRoot());
            }

#ifdef _UI_VMODELDATA_DEBUG
            UserMessage::debug("  Branch treeNode: " + tn->vnode()->strName());
#endif
            //First, we remove the branch contents
            if(tn->numOfChildren())
            {
                int totalRows=tn->attrNum(attrFilter_) + tn->numOfChildren();
                Q_EMIT beginFilterUpdateRemove(this,tn,totalRows);
                tree_->removeChildren(tn);
                Q_EMIT endFilterUpdateRemove(this,tn,totalRows);
            }

            //Second, we add the new contents
            VTreeNode *branch=new VTreeNode(tn->vnode(),tn);
            tree_->buildBranch(filter_->nodes_,tn,branch);
            int chNum=branch->numOfChildren();

            Q_EMIT beginFilterUpdateAdd(this,tn,chNum);
            if(chNum)
            {
               tree_->addBranch(tn,branch);
            }
            Q_EMIT endFilterUpdateAdd(this,tn,chNum);

            Q_ASSERT(branch->numOfChildren() == 0);

            delete branch;
        }
    }

    changeInfo_->clear();

#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("<-- VTreeServer::notifyEndServerSync");
#endif
}

//--------------------------------------------------
// NodeObserver methods
//--------------------------------------------------

void VTreeServer::notifyBeginNodeChange(const VNode* vnode, const std::vector<ecf::Aspect::Type>& aspect, const VNodeChange& change)
{
    if(vnode==NULL)
		return;

    VTreeNode* node=tree_->find(vnode);

	bool attrNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_ATTR) != aspect.end());
	bool nodeNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_NODE) != aspect.end());

	//-----------------------------------------------------------------------
	// The number of attributes changed but the number of nodes is the same!
	//-----------------------------------------------------------------------
    if(node && attrNumCh && !nodeNumCh)
	{
        //We do not deal with the attributes if they were never used for the given node.
        //The first access to the attributes makes them initialised in the tree node.
        if(node->isAttrInitialised())
        {
            //This is the already updated attribute num
            int currentNum=vnode->attrNum(attrFilter_);

            //That is the attribute num we store in the tree node.
            int cachedNum=node->attrNum(attrFilter_);

            Q_ASSERT(cachedNum >= 0);

            int diff=currentNum-cachedNum;
            if(diff != 0)
            {
                Q_EMIT beginAddRemoveAttributes(this,node,currentNum,cachedNum);

                //We update the attribute num in the tree node
                node->updateAttrNum(attrFilter_);
                Q_EMIT endAddRemoveAttributes(this,node,currentNum,cachedNum);
            }
        }
#if 0
        if(change.cachedAttrNum_ != change.attrNum_)
		{
			Q_EMIT beginAddRemoveAttributes(this,node,
					change.attrNum_,change.cachedAttrNum_);
		}
#endif
	}

	//----------------------------------------------------------------------
	// The number of nodes changed but number of attributes is the same!
	//----------------------------------------------------------------------
	else if(!attrNumCh && nodeNumCh)
	{
		//This can never happen
		assert(0);
	}

	//---------------------------------------------------------------------
	// Both the number of nodes and the number of attributes changed!
	//---------------------------------------------------------------------
	else if(attrNumCh && nodeNumCh)
	{
		//This can never happen
		assert(0);
	}

	//---------------------------------------------------------------------
	// The number of attributes and nodes did not change
	//---------------------------------------------------------------------
	else
	{
		//Check the aspects
		for(std::vector<ecf::Aspect::Type>::const_iterator it=aspect.begin(); it != aspect.end(); ++it)
		{
			//Changes in the nodes
			if(*it == ecf::Aspect::STATE || *it == ecf::Aspect::DEFSTATUS ||
			   *it == ecf::Aspect::SUSPENDED)
			{
                if(node)
                {
                    Q_EMIT nodeChanged(this,node);
                }

                changeInfo_->addStateChange(vnode);

#ifdef _UI_VMODELDATA_DEBUG
                UserMessage::debug("   node status changed: " + vnode->strName());
#endif
                //runFilter=true;
			}

			//Changes might affect the icons
			else if (*it == ecf::Aspect::FLAG || *it == ecf::Aspect::SUBMITTABLE ||
					*it == ecf::Aspect::TODAY || *it == ecf::Aspect::TIME ||
					*it == ecf::Aspect::DAY || *it == ecf::Aspect::CRON || *it == ecf::Aspect::DATE)
			{
                if(node)
                {
                    Q_EMIT nodeChanged(this,node);
                }
			}

			//Changes in the attributes
			if(*it == ecf::Aspect::METER ||  *it == ecf::Aspect::EVENT ||
			   *it == ecf::Aspect::LABEL || *it == ecf::Aspect::LIMIT ||
			   *it == ecf::Aspect::EXPR_TRIGGER || *it == ecf::Aspect::EXPR_COMPLETE ||
			   *it == ecf::Aspect::REPEAT || *it == ecf::Aspect::NODE_VARIABLE ||
			   *it == ecf::Aspect::LATE || *it == ecf::Aspect::TODAY || *it == ecf::Aspect::TIME ||
			   *it == ecf::Aspect::DAY || *it == ecf::Aspect::CRON || *it == ecf::Aspect::DATE )
			{
                if(node)
                {
                    Q_EMIT attributesChanged(this,node);
                }
			}
		}
	}

#if 0
	//We do not run the filter now but wait until the sync is finished!
	if(runFilter)
	{
		nodeStateChangeCnt_++;
	}
#endif
}

void VTreeServer::notifyEndNodeChange(const VNode* vnode, const std::vector<ecf::Aspect::Type>& aspect, const VNodeChange& change)
{
    return;
#if 0
    if(vnode==NULL)
		return;

    VTreeNode* node=tree_->find(vnode);
    //Q_ASSERT(node);

    bool attrNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_ATTR) != aspect.end());
	bool nodeNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_NODE) != aspect.end());

	//-----------------------------------------------------------------------
	// The number of attributes changed but the number of nodes is the same!
	//-----------------------------------------------------------------------
    if(node && attrNumCh && !nodeNumCh)
	{
		if(change.cachedAttrNum_ != change.attrNum_)
		{
			Q_EMIT endAddRemoveAttributes(this,node,
					change.attrNum_,change.cachedAttrNum_);
		}
	}

	//----------------------------------------------------------------------
	// The number of nodes changed but number of attributes is the same!
	//----------------------------------------------------------------------
	else if(!attrNumCh && nodeNumCh)
	{
		assert(0);
	}
#endif

}

void VTreeServer::runFilter()
{
    Q_EMIT beginServerClear(this,-1);
    tree_->clear();
    inScan_=true;
    Q_EMIT endServerClear(this,-1);

    Q_ASSERT(filter_);

    if(filter_->isNull())
    {
        tree_->build();
    }

    else
    {
        filter_->update(server_);
        tree_->build(filter_->nodes_);
    }

    Q_EMIT beginServerScan(this, tree_->attrNum(attrFilter_)+tree_->numOfChildren());
    inScan_=false;
    Q_EMIT endServerScan(this, tree_->attrNum(attrFilter_)+tree_->numOfChildren());
}

//==========================================
//
// VTableServer
//
//==========================================

//It takes ownership of the filter

VTableServer::VTableServer(ServerHandler *server,NodeFilterDef* filterDef) :
	VModelServer(server),
	totalNumBeforeClear_(0)

{
	filter_=new TableNodeFilter(filterDef);

	//We has to observe the nodes of the server.
	//server_->addNodeObserver(this);
}

VTableServer::~VTableServer()
{
}

//--------------------------------------------------
// ServerObserver methods
//--------------------------------------------------


void VTableServer::notifyBeginServerScan(ServerHandler* server,const VServerChange& change)
{
    //Q_EMIT beginServerScan(this,change.totalNum_);

	//At this point we do not know how many nodes we will have in the filter!
}

void VTableServer::notifyEndServerScan(ServerHandler* server)
{
	runFilter();
	Q_EMIT endServerScan(this,server->vRoot()->totalNum());

	/*

	//Update the filter using the current server state
	filter_->beginReset(server);

	int realCount=filter_->realMatchCount();

	//The filter pretends it is empty
	//Tell the model to add realCount number of rows!!
	Q_EMIT beginServerScan(this,realCount);

	filter_->endReset();

	Q_EMIT endServerScan(this,realCount);*/
}

void VTableServer::notifyBeginServerClear(ServerHandler* server)
{
	//int n=filter_->matchCount();
	size_t n=server->vRoot()->totalNum();
	totalNumBeforeClear_=n;
	Q_EMIT beginServerClear(this,n);
}

void VTableServer::notifyEndServerClear(ServerHandler* server)
{
	runFilter();

	//int n=filter_->matchCount();
	//filter_->clear();
	Q_EMIT endServerClear(this,totalNumBeforeClear_);
	totalNumBeforeClear_=0;
}

void VTableServer::notifyServerConnectState(ServerHandler* server)
{
	Q_EMIT rerender();
}

void VTableServer::notifyServerActivityChanged(ServerHandler* server)
{
	Q_EMIT dataChanged(this);
}

//This is called when a normal sync (no reset or rescan) is finished. We have delayed the update of the
//filter to this point but now we need to do it.
void VTableServer::notifyEndServerSync(ServerHandler* server)
{
	//if(nodeStateChangeCnt_ >0)
	//{
	//	nodeStateChangeCnt_=0;

		if(!filter_->isNull())
		{
			runFilter();
            //Q_EMIT filterChanged();
		}
	//}
}


void VTableServer::notifyBeginNodeChange(const VNode* node, const std::vector<ecf::Aspect::Type>& types,const VNodeChange&)
{
	/*int n=filter_->matchCount();
	Q_EMIT beginServerClear(this,n);
	filter_->clear();
	Q_EMIT endServerClear(this,n);*/
}

void VTableServer::notifyEndNodeChange(const VNode* node, const std::vector<ecf::Aspect::Type>& types,const VNodeChange&)
{
	//Update the filter using the current server state
	/*filter_->beginReset(server_);
	int realCount=filter_->matchCount();

	//Tell the model to add realCount number of rows!!
	Q_EMIT beginServerScan(this,realCount);

	filter_->endReset();

	Q_EMIT endServerScan(this,realCount);*/

}

//==========================================
//
// VModelData
//
//==========================================

VModelData::VModelData(NodeFilterDef *filterDef,AbstractNodeModel* model) :
		QObject(model),
		serverFilter_(0),
		filterDef_(filterDef),
		model_(model)
{
	connect(filterDef_,SIGNAL(changed()),
			this,SLOT(slotFilterDefChanged()));

	connect(this,SIGNAL(filterDeleteBegin()),
			model_,SLOT(slotFilterDeleteBegin()));

	connect(this,SIGNAL(filterDeleteEnd()),
			model_,SLOT(slotFilterDeleteEnd()));

	//The model relays this signal
    //connect(this,SIGNAL(filterChanged()),
    //		model_,SIGNAL(filterChanged()));

	connect(this,SIGNAL(serverAddBegin(int)),
			model_,SLOT(slotServerAddBegin(int)));

	connect(this,SIGNAL(serverAddEnd()),
			model_,SLOT(slotServerAddEnd()));

	connect(this,SIGNAL(serverRemoveBegin(int)),
			model_,SLOT(slotServerRemoveBegin(int)));

	connect(this,SIGNAL(serverRemoveEnd()),
            model_,SLOT(slotServerRemoveEnd()));

    connect(this,SIGNAL(filterChangeBegun()),
            model_,SIGNAL(filterChangeBegun()));

    connect(this,SIGNAL(filterChangeEnded()),
           model_,SIGNAL(filterChangeEnded()));

}

VModelData::~VModelData()
{
	clear();
}

void VModelData::connectToModel(VModelServer* d)
{
#if 0
    connect(d,SIGNAL(beginAddRemoveAttributes(VModelServer*,const VNode*,int,int)),
		model_,SLOT(slotBeginAddRemoveAttributes(VModelServer*,const VNode*,int,int)));

	connect(d,SIGNAL(endAddRemoveAttributes(VModelServer*,const VNode*,int,int)),
		model_,SLOT(slotEndAddRemoveAttributes(VModelServer*,const VNode*,int,int)));

	connect(d,SIGNAL(nodeChanged(VModelServer*,const VNode*)),
		model_,SLOT(slotNodeChanged(VModelServer*,const VNode*)));

	connect(d,SIGNAL(attributesChanged(VModelServer*,const VNode*)),
		model_,SLOT(slotAttributesChanged(VModelServer*,const VNode*)));
#endif

    connect(d,SIGNAL(dataChanged(VModelServer*)),
        model_,SLOT(slotDataChanged(VModelServer*)));

	connect(d,SIGNAL(beginServerScan(VModelServer*,int)),
		model_,SLOT(slotBeginServerScan(VModelServer*,int)));

	connect(d,SIGNAL(endServerScan(VModelServer*,int)),
		model_,SLOT(slotEndServerScan(VModelServer*,int)));

	connect(d,SIGNAL(beginServerClear(VModelServer*,int)),
		model_,SLOT(slotBeginServerClear(VModelServer*,int)));

	connect(d,SIGNAL(endServerClear(VModelServer*,int)),
		model_,SLOT(slotEndServerClear(VModelServer*,int)));

	//The model relays this signal
    //connect(d,SIGNAL(filterChanged()),
    //	model_,SIGNAL(filterChanged()));

	//The model relays this signal
	connect(d,SIGNAL(rerender()),
		model_,SIGNAL(rerender()));
}

//Completely clear the data and rebuild everything with a new
//ServerFilter.
void VModelData::reset(ServerFilter* serverFilter)
{
	clear();
	serverFilter_=serverFilter;

	init();
}

void VModelData::init()
{
	serverFilter_->addObserver(this);

	for(unsigned int i=0; i < serverFilter_->items().size(); i++)
	{
		if(ServerHandler *server=serverFilter_->items().at(i)->serverHandler())
		{
			add(server);
		}
	}
}

void VModelData::clear()
{
#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("VModelData::clear --> " +  QString::number((qint64)this,16).toStdString());
    qDebug() << "   serverFilter_" << serverFilter_;
#endif

    if(serverFilter_)
		serverFilter_->removeObserver(this);

    serverFilter_=NULL;

	for(int i=0; i < servers_.size(); i++)
	{
		delete servers_.at(i);
	}

	servers_.clear();

#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("<-- VModelData::clear");
#endif
}

VModelServer* VModelData::server(int n) const
{
	return (n >=0 && n < servers_.size())?servers_.at(n):0;
}

ServerHandler* VModelData::serverHandler(int n) const
{
	return (n >=0 && n < servers_.size())?servers_.at(n)->server_:0;
}

int VModelData::indexOfServer(void* idPointer) const
{
	for(unsigned int i=0; i < servers_.size(); i++)
		if(servers_.at(i) == idPointer)
			return i;

	return -1;
}

ServerHandler* VModelData::serverHandler(void* idPointer) const
{
	for(unsigned int i=0; i < servers_.size(); i++)
		if(servers_.at(i) == idPointer)
			return servers_.at(i)->server_;

	return NULL;
}

VModelServer* VModelData::server(const void* idPointer) const
{
	for(unsigned int i=0; i < servers_.size(); i++)
		if(servers_.at(i) == idPointer)
			return servers_.at(i);

	return NULL;
}

VModelServer* VModelData::server(const std::string& name) const
{
    for(unsigned int i=0; i < servers_.size(); i++)
        if(servers_.at(i)->server_->name()  == name)
            return servers_.at(i);

    return NULL;
}

int VModelData::indexOfServer(ServerHandler* s) const
{
	for(unsigned int i=0; i < servers_.size(); i++)
		if(servers_.at(i)->server_ == s)
			return i;
	return -1;
}

int VModelData::numOfNodes(int index) const
{
	if(VModelServer *d=server(index))
	{
		return d->totalNodeNum();
	}
	return 0;
}

void VModelData::runFilter(bool broadcast)
{
    Q_EMIT filterChangeBegun();

    for(unsigned int i=0; i < servers_.size(); i++)
	{
        servers_.at(i)->runFilter();
	}

    Q_EMIT filterChangeEnded();

    /*if(broadcast)
	{
		Q_EMIT filterChanged();
    }*/
}

//ServerFilter observer methods

void VModelData::notifyServerFilterAdded(ServerItem* item)
{
	if(!item)
		return;

	if(ServerHandler *server=item->serverHandler())
	{
		//Notifies the model that a change will happen
		Q_EMIT serverAddBegin(count());

		add(server);

		//Notifies the model that the change has finished
		Q_EMIT serverAddEnd();
		return;
	}
}

void VModelData::notifyServerFilterRemoved(ServerItem* item)
{
	if(!item)
		return;

	int i=0;
	for(std::vector<VModelServer*>::iterator it=servers_.begin(); it!= servers_.end(); ++it)
	{
		if((*it)->server_ == item->serverHandler())
		{
			//Notifies the model that a change will happen
			Q_EMIT serverRemoveBegin(i);

			delete *it;
			servers_.erase(it);

			//Notifies the model that the change has finished
			Q_EMIT serverRemoveEnd();
			return;
		}
		i++;
	}
}

void VModelData::notifyServerFilterChanged(ServerItem* item)
{
	//Q_EMIT dataChanged();
}

void VModelData::notifyServerFilterDelete()
{
#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("VModelData::notifyServerFilterDelete --> " + QString::number((qint64)this,16).toStdString());
#endif

    Q_EMIT filterDeleteBegin();

#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("  filterDeleteBegin emitted");
#endif

	clear();

#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("  filterDeleteEnd emitted");
#endif

	Q_EMIT filterDeleteEnd();

#ifdef _UI_VMODELDATA_DEBUG
    UserMessage::debug("<-- VModelData::notifyServerFilterDelete");
#endif

}

void VModelData::slotFilterDefChanged()
{
	runFilter(true);
}

bool VModelData::isFilterNull() const
{
    for(unsigned int i=0; i < servers_.size(); i++)
    {
        if(servers_.at(i)->filter_)
            return servers_.at(i)->filter_->isNull();
    }

    return true;
}

//==============================================================
//
// VTreeModelData
//
//==============================================================

VTreeModelData::VTreeModelData(NodeFilterDef* filterDef,AttributeFilter* attrFilter,AbstractNodeModel* model) :
        VModelData(filterDef,model),
        attrFilter_(attrFilter)
{

}

void VTreeModelData::connectToModel(VModelServer* s)
{
    VModelData::connectToModel(s);

    VTreeServer* ts=s->treeServer();
    Q_ASSERT(ts);

    connect(ts,SIGNAL(beginAddRemoveAttributes(VTreeServer*,const VTreeNode*,int,int)),
        model_,SLOT(slotBeginAddRemoveAttributes(VTreeServer*,const VTreeNode*,int,int)));

    connect(ts,SIGNAL(endAddRemoveAttributes(VTreeServer*,const VTreeNode*,int,int)),
        model_,SLOT(slotEndAddRemoveAttributes(VTreeServer*,const VTreeNode*,int,int)));

    connect(ts,SIGNAL(nodeChanged(VTreeServer*,const VTreeNode*)),
        model_,SLOT(slotNodeChanged(VTreeServer*,const VTreeNode*)));

    connect(ts,SIGNAL(attributesChanged(VTreeServer*,const VTreeNode*)),
        model_,SLOT(slotAttributesChanged(VTreeServer*,const VTreeNode*)));

    connect(ts,SIGNAL(beginFilterUpdateRemove(VTreeServer*,const VTreeNode*,int)),
            model_,SLOT(slotBeginFilterUpdateRemove(VTreeServer*,const VTreeNode*,int)));

    connect(ts,SIGNAL(endFilterUpdateRemove(VTreeServer*,const VTreeNode*,int)),
            model_,SLOT(slotEndFilterUpdateRemove(VTreeServer*,const VTreeNode*,int)));

    connect(ts,SIGNAL(beginFilterUpdateAdd(VTreeServer*,const VTreeNode*,int)),
            model_,SLOT(slotBeginFilterUpdateAdd(VTreeServer*,const VTreeNode*,int)));

    connect(ts,SIGNAL(endFilterUpdateAdd(VTreeServer*,const VTreeNode*,int)),
            model_,SLOT(slotEndFilterUpdateAdd(VTreeServer*,const VTreeNode*,int)));



   // void endFilterUpdate(VTreeServer*,const std::vector<VNode*>);

}
void VTreeModelData::add(ServerHandler *server)
{
	VModelServer* d=NULL;

    d=new VTreeServer(server,filterDef_,attrFilter_);

    connectToModel(d);

	servers_.push_back(d);
}

//This has to be very fast!!!
bool VTreeModelData::isFiltered(VNode *node) const
{
#if 0
    int id=indexOfServer(node->server());
	if(id != -1)
	{
        return servers_[id]->filter_->isFiltered(node);
	}
#endif
	return true;
}


//==============================================================
//
// VTableModelData
//
//==============================================================

VTableModelData::VTableModelData(NodeFilterDef* filterDef,AbstractNodeModel* model) :
		VModelData(filterDef,model)
{
}

void VTableModelData::add(ServerHandler *server)
{
	VModelServer* d=NULL;

	d=new VTableServer(server,filterDef_);

    connectToModel(d);

	servers_.push_back(d);
}


int VTableModelData::pos(VModelServer* server,VNode** afterNode)
{
	int start=-1;
	*afterNode=NULL;

	if(server)
	{
		ServerHandler *s=server->realServer();
		//*node=s->vRoot()->nodeAt(0);
		start=0;
		for(unsigned int i=0; i < servers_.size(); i++)
		{
			if(servers_.at(i) == server)
			{
				return true;
			}
			size_t n=servers_.at(i)->totalNodeNum();

			start+=n;
			if(n > 0)
			{
				*afterNode=servers_.at(i)->realServer()->vRoot()->nodeAt(n-1);
			}
		}
	}

	return start;
}


bool VTableModelData::identifyInFilter(VModelServer* server,int& start,int& count,VNode** node)
{
	start=-1;
	count=-1;
	*node=NULL;

	if(server)
	{
		ServerHandler *s=server->realServer();

		if(s->vRoot()->totalNum() > 0)
		{
			*node=s->vRoot()->nodeAt(0);
			count=s->vRoot()->totalNum();
			start=0;
			for(unsigned int i=0; i < servers_.size(); i++)
			{
				if(servers_.at(i) == server)
				{
					return true;
				}
				start+=servers_.at(i)->totalNodeNum();
			}
		}
	}

	return false;

	/*start=-1;
	count=-1;
	*node=NULL;

	if(server)
	{
		*node=server->filter_->realMatchAt(0);
		if(*node)
		{
			count=server->filter_->realMatchCount();
			start=0;
			for(unsigned int i=0; i < servers_.size(); i++)
			{
				if(servers_.at(i) == server)
				{
					return true;
				}
				start+=servers_.at(i)->filter_->matchCount();
			}
		}
	}

	return false;*/
}

//This has to be very fast!!!
int VTableModelData::posInFilter(VModelServer* server,const VNode *node) const
{
	if(server)
	{
		int totalRow=0;
		for(unsigned int i=0; i < servers_.size(); i++)
		{
			ServerHandler *s=servers_.at(i)->realServer();
			if(servers_.at(i) == server)
			{
				int pos=node->index();
				if(pos != -1)
				{
					totalRow+=pos;
					return totalRow;
				}
				else
					return -1;
			}
			else
			{
				totalRow+=s->vRoot()->totalNum();
			}
		}
	}

	return -1;
}

//This has to be very fast!!!
int VTableModelData::posInFilter(const VNode *node) const
{
	int serverIdx=indexOfServer(node->server());
	if(serverIdx != -1)
	{
		return posInFilter(servers_.at(serverIdx),node);
	}

	return -1;
}

VNode* VTableModelData::getNodeFromFilter(int totalRow)
{
	int cnt=0;

	if(totalRow < 0)
		return NULL;

	for(unsigned int i=0; i < servers_.size(); i++)
	{
		ServerHandler *s=servers_.at(i)->realServer();

		int pos=totalRow-cnt;
		if(pos < s->vRoot()->totalNum())
		{
			return s->vRoot()->nodeAt(pos);
		}
		cnt+= s->vRoot()->totalNum();
	}

	return NULL;

	/*int cnt=0;

	if(totalRow < 0)
		return NULL;

	for(unsigned int i=0; i < servers_.size(); i++)
	{
		NodeFilter *filter=servers_.at(i)->filter_;
		int pos=totalRow-cnt;
		if(pos < filter->matchCount())
		{
			return filter->matchAt(pos);
		}
		cnt+=filter->matchCount();
	}

	return NULL;*/
}

//Return the number of filtered nodes for the given server
int VTableModelData::numOfFiltered(int index) const
{
	/*if(VModelServer *d=server(index))
	{
		return d->filter_->matchCount();

	}*/
	return 0;
}

//This has to be very fast!!!
bool VTableModelData::isFiltered(VNode *node) const
{
#if 0
    int id=indexOfServer(node->server());
	if(id != -1)
	{
		return servers_.at(id)->filter_->isFiltered(node);
	}
#endif
	return true;
}
