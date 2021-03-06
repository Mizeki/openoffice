/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef ARY_SYMTREE_NODE_HXX
#define ARY_SYMTREE_NODE_HXX


// USED SERVICES
    // BASE CLASSES
    // OTHER



namespace ary
{
namespace symtree
{



/** Represents a node in a tree of symbols like a namespace tree or a
    directory tree.

    @tpl NODE_TRAITS
    Needs to define the types:
        entity_base_type:   The type of the entities in that storage,
                            e.g. ->ary::cpp::CodeEntity.
        id_type:            The type of the ids of those entities,
                            e.g. ->ary::cpp::Ce_id.

    Needs to define the functions:
     1. static entity_base_type &
                            EntityOf_(
                                id_type             i_id );
     2. static symtree::Node<LeNode_Traits> *
                            NodeOf_(
                                const entity_base_type &
                                                    i_entity );
     3. static const String &
                            LocalNameOf_(
                                const entity_base_type &
                                                    i_entity );
     4. static entity_base_type *
                            ParentOf_(
                                const entity_base_type &
                                                    i_entity );
     5. template <class KEY>
        static id_t         Search_(
                                const entity_base_type &
                                                    i_entity,
                                const KEY &         i_localKey );
*/
template <class NODE_TRAITS>
class Node
{
  public:
    typedef Node<NODE_TRAITS>                         node_self;
    typedef typename NODE_TRAITS::entity_base_type    entity_t;
    typedef typename NODE_TRAITS::id_type             id_t;


    // LIFECYCLE
    /// @attention Always needs to be followed by ->Assign_Entity()!
                        Node();
    explicit            Node(
                            entity_t &          i_entity );
    void                Assign_Entity(
                            entity_t &          i_entity );
                        ~Node();
    // INQUIRY
    id_t                Id();
    const String        Name() const;
    int                 Depth() const;
    const entity_t &    Entity() const;
    const node_self *   Parent() const;

    /** Gets a child with a specific name and of a specific type.

        There may be more childs with the same name.

        @return id_t(0), if no matching child is found.
    */
    template <class KEY>
    typename NODE_TRAITS::id_type
                        Search(
                            const KEY &         i_localKey ) const
    {
        // Inline here to workaround SUNW8 compiler bug, works in SUNW12.
        return NODE_TRAITS::Search_(Entity(), i_localKey);
    }


    /** Gets a child with a specific qualified name below this node.

        The child may not exists.
    */
    template <class KEY>
    void                SearchBelow(
                            id_t &              o_return,   // Workaround SUNW8 compiler bug
                            StringVector::const_iterator
                                                i_qualifiedSearchedName_begin,
                            StringVector::const_iterator
                                                i_qualifiedSearchedName_end,
                            const KEY &         i_localKey ) const;

    /** Gets a child with a specific qualified name, either below this node
        or below any of the parent nodes.

        The child may not exists.
    */
    template <class KEY>
    void                SearchUp(
                            id_t &              o_return,   // Workaround SUNW8 compiler bug
                            StringVector::const_iterator
                                                i_qualifiedSearchedName_begin,
                            StringVector::const_iterator
                                                i_qualifiedSearchedName_end,
                            const KEY &         i_localKey ) const;
    // ACCESS
    entity_t &          Entity();
    node_self *         Parent();

  private:
    // Forbid copying:
    Node(const node_self&);
    node_self& operator=(const node_self&);

    // Locals
    void                InitDepth();
    node_self *         Get_Parent() const;
    node_self *         NodeOf(
                            id_t                i_id ) const;

    // DATA
    entity_t *          pEntity;
    int                 nDepth;
};




// IMPLEMENTATION

template <class NODE_TRAITS>
inline const typename Node<NODE_TRAITS>::entity_t &
Node<NODE_TRAITS>::Entity() const
{
    csv_assert(pEntity != 0);
    return *pEntity;
}

template <class NODE_TRAITS>
inline Node<NODE_TRAITS> *
Node<NODE_TRAITS>::NodeOf(id_t i_id) const
{
    if (i_id.IsValid())
        return NODE_TRAITS::NodeOf_(NODE_TRAITS::EntityOf_(i_id));
    return 0;
}

template <class NODE_TRAITS>
inline Node<NODE_TRAITS> *
Node<NODE_TRAITS>::Get_Parent() const
{
    entity_t *
        parent = NODE_TRAITS::ParentOf_(Entity());
    if (parent != 0)
        return NODE_TRAITS::NodeOf_(*parent);
    return 0;
}

template <class NODE_TRAITS>
Node<NODE_TRAITS>::Node()
    :   pEntity(0),
        nDepth(0)
{
}

template <class NODE_TRAITS>
Node<NODE_TRAITS>::Node(entity_t & i_entity)
    :   pEntity(&i_entity),
        nDepth(0)
{
    InitDepth();
}

template <class NODE_TRAITS>
void
Node<NODE_TRAITS>::Assign_Entity(entity_t & i_entity)
{
    pEntity = &i_entity;
    InitDepth();
}

template <class NODE_TRAITS>
Node<NODE_TRAITS>::~Node()
{
}

template <class NODE_TRAITS>
inline typename Node<NODE_TRAITS>::id_t
Node<NODE_TRAITS>::Id()
{
    return NODE_TRAITS::IdOf(Entity());
}

template <class NODE_TRAITS>
inline const String
Node<NODE_TRAITS>::Name() const
{
    return NODE_TRAITS::LocalNameOf_(Entity());
}

template <class NODE_TRAITS>
inline int
Node<NODE_TRAITS>::Depth() const
{
    return nDepth;
}

template <class NODE_TRAITS>
inline const Node<NODE_TRAITS> *
Node<NODE_TRAITS>::Parent() const
{
    return Get_Parent();
}

template <class NODE_TRAITS>
template <class KEY>
void
Node<NODE_TRAITS>::SearchBelow(
                          id_t &              o_return,   // Workaround SUNW8 compiler bug
                          StringVector::const_iterator i_qualifiedSearchedName_begin,
                          StringVector::const_iterator i_qualifiedSearchedName_end,
                          const KEY &                  i_localKey ) const
{
    if (i_qualifiedSearchedName_begin != i_qualifiedSearchedName_end)
    {
        id_t
            next = Search(*i_qualifiedSearchedName_begin);
        if (next.IsValid())
        {
            const node_self *
                subnode = NodeOf(next);
            if (subnode != 0)
            {
                subnode->SearchBelow( o_return,
                                      i_qualifiedSearchedName_begin+1,
                                      i_qualifiedSearchedName_end   ,
                                      i_localKey );
                return;
            }
        }
        o_return = id_t(0);
        return;
    }

    o_return = Search(i_localKey);
}

template <class NODE_TRAITS>
template <class KEY>
void
Node<NODE_TRAITS>::SearchUp(
                          id_t &              o_return,   // Workaround SUNW8 compiler bug
                          StringVector::const_iterator i_qualifiedSearchedName_begin,
                          StringVector::const_iterator i_qualifiedSearchedName_end,
                          const KEY &                  i_localKey ) const
{
    SearchBelow( o_return,
                 i_qualifiedSearchedName_begin,
                 i_qualifiedSearchedName_end,
                 i_localKey );
    if (o_return.IsValid())
        return;

    node_self *
        parent = Get_Parent();
    if (parent != 0)
    {
        parent->SearchUp( o_return,
                          i_qualifiedSearchedName_begin,
                          i_qualifiedSearchedName_end,
                          i_localKey );
    }
}

template <class NODE_TRAITS>
typename Node<NODE_TRAITS>::entity_t &
Node<NODE_TRAITS>::Entity()
{
    csv_assert(pEntity != 0);
    return *pEntity;
}

template <class NODE_TRAITS>
inline Node<NODE_TRAITS> *
Node<NODE_TRAITS>::Parent()
{
    return Get_Parent();
}

template <class NODE_TRAITS>
void
Node<NODE_TRAITS>::InitDepth()
{
    Node<NODE_TRAITS> *
        pp = Get_Parent();
    if (pp != 0)
        nDepth = pp->Depth() + 1;
    else
        nDepth = 0;
}




}   // namespace symtree
}   // namespace ary
#endif
