//StlUtilities.h
//M.L. Miller (Yale Software)
//07/01

#ifndef StlUtilities_HH
#define StlUtilities_HH

#include <iostream>
#include "StiCompositeTreeNode.h"
#include "StiDetector.h"

ostream& operator<<(ostream&, const StiDetector&);

template <class T>
struct StreamNodeName
{
    void operator()(const StiCompositeTreeNode<T>* node) {
	cout <<node->getName()<<endl;
    }
};

template <class T>
struct StreamNodeData
{
    void operator()(const StiCompositeTreeNode<T>* node) {
	T* data = node->getData();
	//cout <<node->getName()<<endl;
	if (data) {cout <<" "<<(*data)<<endl;}
    }
};

template <class T>
struct NodeLessThan
{
    bool operator() (const StiCompositeTreeNode<T>* lhs, const StiCompositeTreeNode<T>* rhs) {
	return ( lhs->getOrderKey() < rhs->getOrderKey() );
    }
    
};

template <class T>
struct SameOrderKey
{
    bool operator() (const StiCompositeTreeNode<T>* rhs) {
	return (morderKey == rhs->getOrderKey() );
    }
    StiOrderKey_t morderKey;
};

template <class T>
struct SameName
{
    bool operator() (const StiCompositeTreeNode<T>* rhs) {
	return (mname == rhs->getName());
    }
    string mname;
};

template <class T>
struct SortDaughters
{
    void operator() (StiCompositeTreeNode<T>* node) {
	if ( node->getChildCount()>0 )
	    {
		sort(node->begin(), node->end(), NodeLessThan<T>());
		for_each(node->begin(), node->end(), SortDaughters() );
	    }
    }
};

template <class T>
struct RecursiveStreamNode
{
    void operator() (StiCompositeTreeNode<T>* node) {
	//cout <<"Parent: "<<node->getName()<<endl;
	cout <<"Name: "<<node->getName()<<endl;
	if (node->getChildCount()>0) {
	    cout <<" Daughters"<<endl;
	    for_each(node->begin(), node->end(), RecursiveStreamNode<T>());
	}
    }
};

template <class T>
class LeafFinder
{
public:
    typedef vector<StiCompositeTreeNode<T>*> tvector_t;
    
    LeafFinder(tvector_t& v)  : vec(v) {};
    
    void operator()(StiCompositeTreeNode<T>* node) {
	if (node->getChildCount()>0) {
	    (*this) = for_each(node->begin(), node->end(), *this);
	}
	else {
	    vec.push_back(node );
	}
    }
    
    void operator=(const LeafFinder& rhs) {copyToThis(rhs);}
    
    tvector_t& vec;

private:
    void copyToThis(const LeafFinder& rhs) {vec = rhs.vec;}
    
    LeafFinder(); //Not implemented
};

template <class T>
inline T gFindClosestOrderKey(T begin, T end, double findThis)
{
    T where = end;
    double min = 1.e100;
    for (T it=begin; it!=end; ++it) {
	double val = fabs(findThis-(*it)->getOrderKey());
	if (val<min) {
	    min=val;
	    where = it;
	}
    }
    return where;
}

template <class T>
struct SameData
{
    T* thedata;
    bool operator()(const StiCompositeTreeNode<T>* rhs) {
	return (thedata == rhs->getData());
    }
};

//Stream a pointer
template <class T>
struct PtrStreamer
{
    void operator()(const T* val) {
	cout <<*val<<endl;
    }
};

#endif
