//C++ : $Id$
/*
-----------------------------------------------------------------------
        XR Core
---------------


Copyright 2010 Daher Alfawares


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at


   http://www.apache.org/licenses/LICENSE-2.0


Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


-----------------------------------------------------------------------
*/


#ifndef __XR_CORE_TREE_HPP__
#define __XR_CORE_TREE_HPP__


#include <stack>


#ifdef __XR_CORE_TREE__BOOST_SERIALIZATION__
#include <boost/serialization/list.hpp>
#else
#include <list>
#endif //__XR_CORE_TREE__BOOST_SERIALIZATION__


namespace xr { namespace core
        {
                ////////////////////////
                /// forward decleration for the tree object.
                /// A tree data structure emulates a hierarchical tree structure with a list of linked nodes.
                /// It is an acyclic connected graph where each node has a list of zero or more children nodes,
                /// and at most one parent node. Each node is a tree by itself.
        template<typename _Vt> class tree;


        namespace imp
                {


        struct base_iterator_tag
                {
                };


        struct value_iterator_tag
                : public base_iterator_tag
                {
                };


        struct node_iterator_tag
                : public base_iterator_tag
                {
                };


                ////////////////////////
                /// value iterator base. uses a node iterator to allow direct access to the value.
                /// _Tt : Tree Type.
                /// _Vt : Value Type.
                /// _It : Iterator Type.
        template<typename _Tt, typename _Vt, typename _It> struct value_iterator
                : public value_iterator_tag
                , public _It
                { //TODO: implement this iterator.
                };


                ////////////////////////
                /// pre order iterator. an iterator that runs through all the nodes of the tree.
                /// _Tt : Tree Type.
                /// _Vt : Value Type.
                /// _It : Iterator Type.
        template<typename _Tt, typename _Vt, typename _It> struct preorder_iterator
                : public value_iterator_tag
                {
                typedef preorder_iterator<_Tt,_Vt,_It>        instance;
                typedef preorder_iterator<_Tt,_Vt,_It>*       pointer;
                typedef preorder_iterator<_Tt,_Vt,_It>&       reference;
                typedef _Tt                                   value_type;
                typedef std::forward_iterator_tag             iterator_category;
                typedef std::streamsize                       difference_type;
                typedef value_iterator_tag                    iterator_traits;


                private:
                typedef _It                                   internal_iterator;


                public:


                        ////////////////////////
                        /// construct empty.
                preorder_iterator(){}


                        ////////////////////////
                        /// construct an actual iterator.
                preorder_iterator( _Tt* _Tree, internal_iterator _Iter )
                        {
                        this->internals.push( std::make_pair( _Tree, _Iter ) );
                        }


                        ////////////////////////
                        /// Assign to another iterator.
                reference operator = ( const instance& _Ref )
                        {
                        this->internals = _Ref.internals;
                        return *this;
                        }


                        ////////////////////////
                        /// non equality check
                bool operator != ( reference _Right ) const
                        {
                        if( this->internals.empty() || _Right.internals.empty() )
                                return true; // empty iterators are not equal.


                        if( this->internals.top().first != _Right.internals.top().first )
                                return true; // different ancestor are not equal.


                        return this->internals.top().second != _Right.internals.top().second;
                        }


                        ////////////////////////
                        /// equality check
                bool operator == ( reference _Right ) const
                        {
                        if( this->internals.empty() || _Right.internals.empty() )
                                return false; // empty iterators are not equal.


                        if( this->internals.top().first != _Right.internals.top().first )
                                return false; // different ancestor are not equal.


                        return this->internals.top().second == _Right.internals.top().second;
                        }


                        ////////////////////////
                        /// increment operator, this will go thru every node in the tree.
                reference operator ++()
                        {
                        //assert( !this->internals.empty() );


                        _Tt & Cur = *this->internals.top().second;
                        if( Cur.leaf() )
                                {
                                ++this->internals.top().second;


                                        // a leaf could be the end of a branch.
                                while( this->depth() &&
                                        this->internals.top().second == this->internals.top().first->end() )
                                        {
                                        this->internals.pop();
                                        ++this->internals.top().second;
                                        }
                                }
                        else
                                {
                                this->internals.push( std::make_pair( &Cur, Cur.begin() ) );
                                }


                        return *this;
                        }


                        ////////////////////////
                        /// postfix increment operator
                instance operator ++(int)
                        {
                        instance Copy = *this;


                        if( this->depth() )
                                {
                                        // treat as a leaf.
                                ++this->internals.top().second;


                                        // a leaf could be the end of a branch.
                                while( this->depth() &&
                                        this->internals.top().second == this->internals.top().first->end() )
                                        {
                                        this->internals.pop();
                                        ++this->internals.top().second;
                                        }
                                }


                        return Copy;
                        }


                        ////////////////////////
                        /// dereferences into a value type.
                _Vt& operator *()
                        {
                        return (*this->internals.top().second).value();
                        }


                        ////////////////////////
                        /// member access operator.
                _Vt* operator ->()
                        {
                        return &this->internals.top().second->value();
                        }


                        ////////////////////////
                        /// member access operator.
                const _Vt* operator ->() const
                        {
                        return &this->internals.top().second->value();
                        }


                        ////////////////////////
                        /// this will return the depth of the current node in the tree hirarchy.
                std::size_t depth() const
                        {
                        return this->internals.size() - 1;
                        }


                private:




                std::stack< std::pair<_Tt*, internal_iterator> > internals;
                };


                } /* imp */


                /////////////////////////
                /// core::tree
                /// ---------------------
                /// The actual definition of the tree class template.
                /// A tree data structure emulates a hierarchical tree structure with a list of linked nodes.
                /// It is an acyclic connected graph where each node has a list of zero or more children nodes,
                /// and at most one parent node. Each node is a tree by itself.
                ///
                /// _Vt : Value Type.
                /// _Ct : Container Type.
                /// _Tt : Tree Type.
        template<typename _Vt> class tree
                {
                typedef tree<_Vt>                 _Tt;                                 ///< tree type.
                typedef std::list< _Tt >         _Ct;                                  ///< internal container type.


                public:
                typedef _Vt                                      value_type;           ///< the value type of the container.
                typedef typename _Ct::iterator                   iterator;             ///< first level iterator. implicit node_iterator_tag
                typedef typename _Ct::const_iterator             const_iterator;       ///< constant iterator. implicit node_iterator_tag
                typedef _Tt&                                     reference;            ///< reference to a tree object.
                typedef const _Tt&                               const_reference;      ///< constant reference to a tree object.
                typedef typename _Ct::size_type                  size_type;            ///< size type.


                typedef imp::preorder_iterator<_Tt,_Vt,iterator>                      preorder_iterator;        ///< pre order iterator.
                typedef imp::preorder_iterator<const _Tt,const _Vt,const_iterator>    const_preorder_iterator;  ///< const pre order iterator.


                public:
                        
                        ////////////////////////////
                        /// construct an empty tree.
                tree()
                        {
                        }


                tree( const _Tt& _Tt )
                        {
                        this->list = _Tt.list;
                        this->val = _Tt.val;
                        }


                        ////////////////////////////
                        /// construct from a value.
                explicit tree( const _Vt& _Value )
                        {
                        this->val = _Value;
                        }


                        ////////////////////////////
                        /// pushes a node to the end of the tree.
                _Tt& push_back( const _Vt& _Value )
                        {
                        this->list.push_back( _Tt(_Value) );
                        return this->list.back();
                        }


                        ////////////////////////////
                        /// pushes a node to the begining of the tree.
                _Tt& push_front( const _Vt& _Value )
                        {
                        this->list.push_front( _Tt(_Value) );
                        return this->list.front();
                        }


                        ////////////////////////////
                        /// treat as a value type.
                operator _Vt()
                        {
                        return this->val;
                        }


                        ////////////////////////////
                        /// alternatively, allow value() to return a reference.
                _Vt& value()
                        {
                        return this->val;
                        }


                        ////////////////////////////
                        /// a constant value() returns a const reference.
                const _Vt& value() const
                        {
                        return this->val;
                        }


                        ////////////////////////////
                        /// treet as a const value type.
                operator const _Vt() const
                        {
                        return this->val;
                        }


                        ////////////////////////////
                        /// returns true if the tree is empty.
                bool empty() const
                        {
                        return this->list.empty();
                        }


                        ////////////////////////////
                        /// returns true if this node is a leaf or a full tree.
                bool leaf() const
                        { // an empty list is a leaf.
                        return this->empty();
                        }


                        ////////////////////////////
                        /// all the elements in the tree are dropped: their destructors are called,
                        /// and then they are removed from the list container, leaving it with a size of 0.
                void clear()
                        {
                        this->list.clear();
                        }


                        //
                        // begin.
                iterator                begin()                           { return this->list.begin(); }
                const_iterator            begin() const                   { return this->list.begin(); }


                        //
                        // end.
                iterator                end()                             { return this->list.end(); }
                const_iterator            end() const                     { return this->list.end(); }


                        //
                        // begin pre order.
                preorder_iterator        begin_preorder()                 { return preorder_iterator( this, this->begin() ); }
                const_preorder_iterator    begin_preorder() const         { return const_preorder_iterator( this, this->begin() ); }


                        //
                        // end pre order.
                preorder_iterator        end_preorder()                   { return preorder_iterator( this, this->end() ); }
                const_preorder_iterator    end_preorder() const           { return const_preorder_iterator( this, this->end() ); }


                        ////////////////////
                        /// the depth of an iterator within the tree.
                size_type depth( preorder_iterator _Iter ) const          { return _Iter.depth(); }
                
                        ////////////////////
                        /// constant version of depth().
                size_type depth( const_preorder_iterator _Iter ) const    { return _Iter.depth(); }


                        ////////////////////
                        /// the value at the back of the first level.
                _Vt& back()                                               { return this->list.back(); }
                
                        ////////////////////
                        /// constant version of back().
                const _Vt& back() const                                   { return this->list.back(); }


                        ////////////////////
                        /// the back node of the first level.
                _Tt& back_node()                                          { return this->list.back(); }
                
                        ////////////////////
                        /// constant version of back_node().
                const _Tt& back_node() const                              { return this->list.back(); }


                        /////////////////////////////////////////////////////////////////
                        /// returns the first element in the first level that satisfies pred.
                template<typename _Predicate> iterator find_if( _Predicate _Pred )
                        {
                        return std::find_if( this->begin(), this->end(), _Pred );
                        }


#ifdef __XR_CORE_TREE__BOOST_SERIALIZATION__


                        ////////////////////////////////////////////////////////////////
                        /// boost serialization
                template <typename _ArchiveType> void serialize( _ArchiveType& _Archive, const unsigned int _Version )
                        {
                        _Archive & this->val;
                        _Archive & this->list;
                        }


#endif //__XR_CORE_TREE__BOOST_SERIALIZATION__


                private:
                _Ct        list;            ///< the internal list of first level nodes.
                _Vt        val;             ///< the value of the current node.
                };




        } /* core */ } /* xr */


#endif// __XR_CORE_TREE_HPP__

