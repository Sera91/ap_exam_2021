/***********************************************************************************************************
 * Library written by Serafina Di Gioia for the exam of the "Advanced Programming" course (A.Y. 2021-2022) *
 ***********************************************************************************************************
 * In this project, we write a pool for fast linked lists. 
 * The pool stores each node in a `std::vector<node_t>`. 
 * The "address" of a node is `1+idx`, where `idx` is the index where the node is stored in the vector. 
 * This trick allows us to use address `0` as `end`, so we can use unsigned integers type. 
 * The first node stored in the vector will be put at `idx == 0`, but it will be referenced as `1`.
 *********************************************************************************************************/ 


#pragma once
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

/*The class iterator gets 2 members:
 *  - a pointer to pool
 *  - an index
 * Inside this class I define:
 *  -reference operator (operator*())
 *  -pre-increment operator
 *  -post-increment operator
 *  -logical operator ==
 *  -logical operator !=
 *  -getIndex() method
 *  -getNext() method
***/

template <typename node_t, typename T, typename N>
class _iterator {
   std::vector<node_t> * pool;
   N index_node;

  public:
   using value_type = T;
   using reference = value_type&;
   using pointer = value_type*;
   using difference_type = N;
   using iterator_category = std::forward_iterator_tag;


   //ITERATOR constructor:
   _iterator(std::vector<node_t> & p, N x) : pool(&p), index_node(x) {}
   

   //REFERENCE OPERATOR:
   //returns the value of the given pool node
   reference operator*() const {
    	return (*pool)[index_node - 1 ].value;
   }
   

   //OPERATOR pre-increment:
   //cycles over the nodes of the linked list iterating through next indexes
   _iterator& operator++() {
      index_node = (*pool)[index_node - 1 ].next ; 
    	return *this;
   }

   //OPERATOR post-increment:
   _iterator operator++(int) {
    	_iterator tmp(*this);
    	++*this;
    	return tmp;
   }

   //OPERATOR pointer
   pointer operator->() const {
    	return &**this;
   }

   //getNext method:
   // returns the "address" of the node pointed by the read pool node
   N& getNext() const {
    	return (*pool)[index_node - 1 ].next;
   }
   //get_index() method:
   // returns the "address" of the node in the pool
   N get_index() const {return index_node;}

   friend bool operator==(const _iterator& x, const _iterator& y) {
      return x.index_node == y.index_node;
   }

   friend bool operator!=(const _iterator& x, const _iterator& y) {
      return !(x == y);
   }

};

template <typename T, typename N = std::size_t>
class list_pool{ 
  struct node_t{
    T value;
    N next;
  };


  std::vector<node_t> pool;
  using list_type = N;
  using value_type = T;
  using size_type = typename std::vector<node_t>::size_type;
  list_type free_node_list {end()}; // at the beginning, it is empty

  node_t& node(list_type x) noexcept { return pool[x-1]; }
  const node_t& node(list_type x) const noexcept { return pool[x-1]; }

/*node_t& node(list_type x) noexcept { return pool.at(x-1); }
  const node_t& node(list_type x) const noexcept { return pool.at(x-1); }
*/
 
 public:
  // list_pool() {*this = list_pool(0);};
  list_pool() = default;
  explicit list_pool(size_type n): pool(0) { reserve(n); }; // reserve the space in memory for n nodes in the pool, leaving the size of the pool equal to 0


  //Some constructors and operators useful to copy the vector pool, with all the linked lists inside:

  //move constructor
  list_pool(list_pool&& src) :pool{std::move(src.pool)}{
        std::cout<<"Move constructor "<<std::endl;
  }

  //move assignment
  list_pool& operator=(list_pool&& src) {
        std::cout<<"Move assignment... "<<std::endl;
        pool=std::move(src.pool);
        return *this;
  }

  // copy constructor 
  list_pool(const list_pool& src):pool{src.pool} {
        std::cout<<"Copy constructor "<<std::endl;
  }

  // copy assignment 
  list_pool& operator=(const list_pool& src) {
        std::cout<<"Copy assignment... "<<std::endl;
        pool.clear();
        auto tmp=src;
        (*this) = std::move(tmp);
        return *this;
  }
    

  using iterator = _iterator<node_t, T, N>;
  using const_iterator = _iterator<node_t, const T, N>;


  iterator begin(list_type x) noexcept { return iterator{ pool, x }; };
  iterator end(list_type ) noexcept { return iterator{ pool, end()}; }; // this is not a typo
    
  const_iterator begin(list_type x) const noexcept { return const_iterator{ pool, x}; } ;
  const_iterator end(list_type ) const noexcept { return const_iterator{pool, end()}; } ;
  
  const_iterator cbegin(list_type x) const noexcept { return const_iterator{ pool, x}; } ;
  const_iterator cend(list_type ) const noexcept { return const_iterator{ pool, end()}; } ;
    
  list_type new_list() noexcept { return list_type(0); } ; // return an empty list

  // return the capacity of the pool
  size_type capacity() const noexcept { return pool.capacity(); }; 

  // reserve n nodes in the pool
  void reserve(size_type n){
    if (n > capacity() )
       pool.reserve(n);
  }  

  bool is_empty(list_type x) const noexcept { return x == end();};

  list_type end() const noexcept { return list_type(0); }

  T& value(list_type x) noexcept {
    return node(x).value;
  } 

  const T& value(list_type x) const noexcept {
    return node(x).value;
  }

  list_type& next(list_type x) noexcept {
    return node(x).next;
  }


  const list_type& next(list_type x) const noexcept {
    return node(x).next;
  }


  
  /**************************************************************
   *** Utility function ADD_NODE()
   *** takes as arguments:
   ***  - the value of the new node, to insert in the linked list
   ***  - the index of the node pointed by the new node
   *** returns:
   *** the next index.
  ***************************************************************/
  list_type add_node(value_type nodeval, list_type nodenext) {
    auto new_node = node_t {nodeval, nodenext};
    list_type retindex;
    //I check if 'free_node_list' is empty
    if (is_empty(free_node_list)) {
        
        //if 'free_node_list' is empty (so there are no free nodes in the pool):
        // I call the utility function 'reserve()'
        // that checks the capacity of the pool vector
        // and if necessary I enlarge it :
        reserve(pool.size() + 1);
        //I insert the new node in the pool
        pool.push_back(new_node);
        retindex = pool.size();       
    }
    else {

        //I take the first index in the free_node_list
        //which is the 'address' of the last node freed
        retindex = free_node_list;
        //I assign the value of the new node to this node
        node(retindex).value = nodeval;
        //I modify the next of this node to point to the next of new node
        //second argument of this function
        auto tmp = node(retindex).next ;
        
        node(retindex).next = nodenext;
        free_node_list = tmp ;
    }
    //std::cout << "RITORNO CON " << retindex << std::endl;
    return retindex;
  }


  /*********************************************************************
   Templated version of the push-front() function, taking as arguments:
   *   - the value of the node, passed as r-val defined by new template X
   *   - the index of the head of the linked list 
   *  and inserting a new node at the head of the assigned linked list
   *******************************************************************/
  template<typename X>
  list_type _push_front(X&& val, list_type head) {
      list_type new_head = add_node(std::forward<X>(val), head);
      return new_head;
      
  }


  /*******************************************************************
   *  Version 1 of the push-front function, taking as arguments:
   *   - the value of the node, passed as l-val 
   *   - the index of the head of the linked list 
   *  and inserting a new node at the head of the assigned linked list
   *******************************************************************/
  list_type push_front(const T& val, list_type head) {
      return _push_front(val, head);
      //list_type new_head = add_node(val, head);
      //return new_head;
      
  }

   /********************************************************************
   *  Version 2 of the push-front function, taking as arguments:
   *   - the value of the node, passed as r-val 
   *   - the index of the head of the linked list 
   *  and inserting a new node at the head of the assigned linked list
   ********************************************************************/
  list_type push_front(T&& val, list_type head) {
      return _push_front(std::move(val), head);
      //list_type new_head = add_node(std::move(val), head);
      //return new_head;
      
  }

    /*********************************************************************
   Templated version of the push-back function, taking as arguments:
   *   - the value of the node, passed as r-val defined by new template X
   *   - the index of the head of the linked list 
   *  and inserting a new node at the back of the assigned linked list
   *******************************************************************/
   template <typename X>
   list_type _push_back(X&& val, list_type head) {
    //STEP 1: I check if the linked list pointed by head is empty
    if (is_empty(head)) {
        //if the list is empty:
        //I call directly the utility function add_node() to insert the new node
        head = add_node(std::forward<X>(val), end());
    } 
    else {
        //if the list is not empty:
        //I call the iterator begin()
        auto itr = begin(head);
        //and I cycle over the list elements:
        while(itr.getNext() != end()) {
            //std::cout << *itr << std::endl;
            ++itr;
        }
        //then I get the index of the last node in the linked list 
        auto myindex = itr.get_index();
        //and I call the utility function add_node 
        //to insert the new node in the last position of the linked list
        next(myindex) = std::move(add_node(std::forward<X>(val), end()));;
     
    }
    //At the end I return the index of the head of the linked list
    return head;
  }

  /*********************************************************************
   *  Version 1 of the push-back function, taking as arguments:
   *   - the value of the node, passed as l-val 
   *   - the index of the head of the linked list 
   *  and inserting a new node at the back of the assigned linked list
   *********************************************************************/

  list_type push_back(const T& val, list_type head) {
  	return _push_back(val, head);

  }
  


  /*********************************************************************
   Version 2 of the push-back function, taking as arguments:
   *   - the value of the node, passed as r-val 
   *   - the index of the head of the linked list 
   *  and inserting a new node at the back of the assigned linked list
   ********************************************************************/
  list_type push_back(T&& val, list_type head) {
  	return _push_back(std::move(val), head);
  }
  
  
  /********************************************************************
   * Function to free the first node of the linked list
   * It takes as argument the index of the head of the linked list
   * changes the next(index) to point to the free_node_list
   * adds the given index to the free_node_list
   * and returns the index of the new head
   ********************************************************************/
  list_type free(list_type x) { 
    list_type  new_head;
    if (x != end()) {
    if (node(x).next != end()){
      new_head = node(x).next;
    }
    else {
      new_head = list_type(0);
    }
    if(!is_empty(free_node_list))
        node(x).next  = free_node_list;
    else
        node(x).next  = end();
    free_node_list =  x;
    } else { new_head = x;}
    return new_head;
  } 

  /***Function to free the entire linked list
   ***base on the 'free()' function
   ***/

  list_type free_list(list_type x){// free entire list
    while (!is_empty(x)) x = free(x);
    return x;
  }
};
