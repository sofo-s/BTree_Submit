//
// Created by 閮戞枃閼?on 2019-03-09.
//

#include "utility.hpp"
#include <functional>
#include <cstddef>
#include "exception.hpp"
using namespace std;
namespace sjtu {
template <class Key, class Value, class Compare = std::less<Key> >
class BTree {
	friend class node;
	friend class node_y;

 private:
 	static const int m=1000;
 	static const int l=200;
 	class node{
 		friend class BTree;
 		friend class node_y;
 	private:
 		ssize_t pos;
 		Key key[m+1];
 		ssize_t son[m+1];
 		ssize_t fa;
 		size_t size;
 		bool kid; //0=leaf 1=internal 
 		node(){
 			pos=fa=0;
 			size=0;
 			kid=0;
 			for(int i=0;i<m;i++) son[i]=0;
		}
	};
	 class node_y{
	 	friend class BTree;
 		friend class node;
	private: 
		ssize_t pos;
	 	ssize_t pre;
	 	size_t size;
	 	ssize_t fa;
	 	ssize_t next;
	 	Key key[l+1];
		Value val[l+1];
	 	node_y(){
	 		size=0;
		 	pos=pre=next=fa=0;
		 }
	};
	FILE* bt;
	struct info{	
		ssize_t root;
		ssize_t head;
		ssize_t tail;
		ssize_t end;
		size_t size;
		info(){

			root=head=tail=0;
			size=0;
			end=0;
		}
	};
	info *mation;
	inline void fileopen(){
		if(bt) fclose(bt);
		bt=fopen("btree","rb+");
		if(bt==nullptr){
			bt=fopen("btree","w");
			fclose(bt);
			bt=fopen("btree","rb+");
			fseek(bt,0,0);
			fwrite(mation,sizeof(info),1,bt);
		}
		else {
			fseek(bt,0,0);
			fread(mation,sizeof(info),1,bt);
		}
	}
	pair<ssize_t,int> locate(const Key &key,ssize_t pos){
		node find;
		fseek(bt,pos,0);
		fread(&find,sizeof(node),1,bt);
		int i;
		for(i=1;i<find.size&&key>find.key[i];i++);
		if(find.key[i]!=key) i--;
		if(find.kid) return locate(key,find.son[i]);
		else {
			node_y aim;
			fseek(bt,find.son[i],0);
			fread(&aim,sizeof(node_y),1,bt);
			int j;
			for(j=0;j<aim.size&&key>aim.key[j];j++);
			pair<ssize_t,int> ok(aim.pos,j-1);
			return ok;
		}
	}
	void crash_leaf(node_y& aim){
		node_y pro;
		if(aim.pre==0&&aim.next==0){
			crash_leaf1(aim,pro);
			node newroot;
			newroot.pos=mation->end;
			mation->end+=sizeof(node);
			mation->root=newroot.pos;
			mation->tail=pro.pos;
			newroot.key[1]=pro.key[0];
			newroot.son[0]=aim.pos;
			newroot.son[1]=pro.pos;
			newroot.size=2;
			aim.fa=newroot.pos;
			pro.fa=newroot.pos;
			fseek(bt,aim.pos,0);
			fwrite(&aim,sizeof(node_y),1,bt);
			fseek(bt,pro.pos,0);
			fwrite(&pro,sizeof(node_y),1,bt);
		    fseek(bt,newroot.pos,0);
			fwrite(&newroot,sizeof(node),1,bt);
			return;
		} 
		else{/*
			if(aim.next!=0){
				fseek(bt,aim.next,0);
				fread(&pro,sizeof(node_y),1,bt);
				if(pro.size<l){
					for(int i=pro.size-1;i>=0;i--) {
					    pro.key[i+1]=pro.key[i];
					    pro.val[i+1]=pro.val[i];
					}
					pro.val[0]=aim.val[l];
					pro.key[0]=aim.key[l];
					pro.size++;
					aim.size--;
					node fat;
					fseek(bt,pro.fa,0);
					fread(&fat,sizeof(node),1,bt);
					int i;
					for(i=1;i<fat.size&&fat.key[i]<pro.key[1];i++);
					if(fat.key[i]==pro.key[1]) fat.key[i]=pro.key[0];
					else update(fat,pro.key[0],pro.key[1]);
					fseek(bt,aim.pos,0);
					fwrite(&aim,sizeof(node_y),1,bt);
					fseek(bt,pro.pos,0);
					fwrite(&pro,sizeof(node_y),1,bt);
					fseek(bt,fat.pos,0);
					fwrite(&fat,sizeof(node),1,bt);
					return;
				}
			}
			if(aim.pre!=0){
				fseek(bt,aim.pre,0);
				fread(&pro,sizeof(node_y),1,bt);
				if(pro.size<l){
					size_t k=(l-pro.size)/2+1;
					for(int i=0;i<k;i++){
						pro.key[pro.size+i]=aim.key[i];
						pro.val[pro.size+i]=aim.val[i];
					}
					for(int i=k;i<aim.size;i++) {
					    aim.key[i-k]=aim.key[i];
					    aim.val[i-k]=aim.val[i];
					}
					aim.size-=k;
					pro.size+=k;
					node fat;
					fseek(bt,aim.fa,0);
					fread(&fat,sizeof(node),1,bt);
					int i;
					for(i=1;i<fat.size&&fat.key[i]<pro.key[pro.size-k];i++);
					if(fat.key[i]==pro.key[pro.size-k]) fat.key[i]=aim.key[0];
					else update(fat,aim.key[0],pro.key[pro.size-k]);
					fseek(bt,aim.pos,0);
					fwrite(&aim,sizeof(node_y),1,bt);
					fseek(bt,pro.pos,0);
					fwrite(&pro,sizeof(node_y),1,bt);
					fseek(bt,fat.pos,0);
					fwrite(&fat,sizeof(node),1,bt);
					return;
				}	
			}*/ 
			crash_leaf1(aim,pro);
			node fat;
			fseek(bt,aim.fa,0);
			fread(&fat,sizeof(node),1,bt);
			int i;
			for(i=1;i<fat.size&&fat.key[i]<aim.key[0];i++);
			if(fat.key[i]!=aim.key[0]) i=0; 
			for(int j=fat.size-1;j>i;j--) {
				fat.key[j+1]=fat.key[j];
				fat.son[j+1]=fat.son[j];
			}
			fat.key[i+1]=pro.key[0];
			fat.son[i+1]=pro.pos;
			fat.size++;
			if(fat.size>m) {
				node_y min;
				fseek(bt,fat.son[0],0);
				fread(&min,sizeof(node_y),1,bt);
				crash_node(fat,min.key[0]);
			}
			fseek(bt,aim.pos,0);
			fwrite(&aim,sizeof(node_y),1,bt);
			fseek(bt,pro.pos,0);
			fwrite(&pro,sizeof(node_y),1,bt);
			fseek(bt,fat.pos,0);
			fwrite(&fat,sizeof(node),1,bt);
			return;
		}
	}
	void update(node cur,Key min,Key od){
		if(cur.fa==0) return;
		node fat;
		fseek(bt,cur.fa,0);
		fread(&fat,sizeof(node),1,bt);
		int i;
		for(i=1;i<fat.size&&fat.key[i]<od;i++);
		if(fat.key[i]==od) fat.key[i]=min;
		else update(fat,min,od);
		fseek(bt,fat.pos,0);
		fwrite(&fat,sizeof(node),1,bt);
	}
	void crash_leaf1(node_y& aim,node_y& pro){
		pro.pos=mation->end;
		mation->end+=sizeof(node_y);
		for(int i=l/2;i<=l;i++){
			pro.val[i-l/2]=aim.val[i];
			pro.key[i-l/2]=aim.key[i];
		}
		aim.size=l/2;
		pro.size=l-l/2+1;
		pro.pre=aim.pos;
		pro.next=aim.next;
		pro.fa=aim.fa;
		if(aim.next){
			node_y n;
			fseek(bt,aim.next,0);
			fread(&n,sizeof(node_y),1,bt);
			n.pre=pro.pos;
			fseek(bt,aim.next,0);
			fwrite(&n,sizeof(node_y),1,bt);
		}
		else{
			mation->tail=pro.pos;
			
		}
		fseek(bt,0,0);
		fwrite(mation,sizeof(info),1,bt);
		aim.next=pro.pos;
	} 
	void crash_node(node aim,Key min){
		if(aim.fa==0){
			node newroot;
			newroot.pos=mation->end;
			mation->end+=sizeof(node);
			newroot.kid=1;
			node pro;
			for(int j=m/2;j<=m;j++){
				pro.key[m/2-j]=aim.key[j];
				pro.son[m/2-j]=aim.son[j];
			}//key[0]没用 
			pro.pos=mation->end;
			mation->end+=sizeof(node); 
			aim.size-=m-m/2;
			pro.size=m/2;
			newroot.size=2;
			newroot.son[0]=aim.pos;
			newroot.son[1]=pro.pos;
			aim.fa=newroot.pos;
			pro.fa=newroot.pos;
			newroot.key[1]=pro.key[0];
			mation->root=newroot.pos;
			fseek(bt,aim.pos,0);
			fwrite(&aim,sizeof(node),1,bt);
			fseek(bt,pro.pos,0);
			fwrite(&pro,sizeof(node),1,bt);
			fseek(bt,newroot.pos,0);
			fwrite(&newroot,sizeof(node),1,bt);
			fseek(bt,0,0);
			fwrite(mation,sizeof(info),1,bt);
			return;
		}
		node fat;
		fseek(bt,aim.fa,0);
		fread(&fat,sizeof(node),1,bt);
		int i;
		for(i=1;i<fat.size&&fat.key[i]<min;i++);
			if(fat.key[i]!=min) i=0;
			for(int j=fat.size-1;j>i;j--){
				fat.key[j+1]=fat.key[j];
				fat.son[j+1]=fat.son[j];
			}
			node pro;
			for(int j=m/2;j<=m;j++){
				pro.key[m/2-j]=aim.key[j];
				pro.son[m/2-j]=aim.son[j];
			}//key[0]没用 
			pro.pos=mation->end;
			mation->end+=sizeof(node); 
			aim.size-=m-m/2;
			pro.size=m/2;
			pro.fa=aim.fa;
			fat.key[i+1]=pro.key[0];
			fat.son[i+1]=pro.pos; 
			fat.size++;
			if(fat.size>m) crash_node(fat,min);
			fseek(bt,aim.pos,0);
			fwrite(&aim,sizeof(node),1,bt);
			fseek(bt,pro.pos,0);
			fwrite(&pro,sizeof(node),1,bt);
			fseek(bt,fat.pos,0);
			fwrite(&fat,sizeof(node),1,bt);
			fseek(bt,0,0);
			fwrite(mation,sizeof(info),1,bt);
	}
  // Your private members go here
 public:typedef pair<const Key, Value> value_type;
//  typedef pair<const Key, Value> value_type;
  class const_iterator;
  class iterator {
   private:
    // Your private members go here
   public:
    iterator() {
      // TODO Default Constructor
    }
    iterator(const iterator& other) {
      // TODO Copy Constructor
    }
    // Return a new iterator which points to the n-next elements
    iterator operator++(int) {
      // Todo iterator++
    }
    iterator& operator++() {
      // Todo ++iterator
    }
    iterator operator--(int) {
      // Todo iterator--
    }
    iterator& operator--() {
      // Todo --iterator
    }
    // Overloaded of operator '==' and '!='
    // Check whether the iterators are same
    value_type& operator*() const {
      // Todo operator*, return the <K,V> of iterator
    }
    bool operator==(const iterator& rhs) const {
      // Todo operator ==
    }
    bool operator==(const const_iterator& rhs) const {
      // Todo operator ==
    }
    bool operator!=(const iterator& rhs) const {
      // Todo operator !=
    }
    bool operator!=(const const_iterator& rhs) const {
      // Todo operator !=
    }
    value_type* operator->() const noexcept {
      /**
       * for the support of it->first.
       * See
       * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
       * for help.
       */
    }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // Your private members go here
   public:
    const_iterator() {
      // TODO
    }
    const_iterator(const const_iterator& other) {
      // TODO
    }
    const_iterator(const iterator& other) {
      // TODO
    }
    // And other methods in iterator, please fill by yourself.
  };
  // Default Constructor and Copy Constructor
  BTree() {
  	mation=new info;
	mation->end=sizeof(info); 
	bt=nullptr;
	fileopen();
    // Todo Default
  }
  /*BTree(const BTree& other) {
  	  bt=nullptr;
  	  fileopen();
  	  mation=other.mation;
  	  char *b1;
	  b1=new char[other.mation->end+1];
	  other.fseek(bt,sizeof(info),0);
	  other.bt.read(b1,mation->end);
	  bt.write(b1,mation->end); 
    // Todo Copy
  }*/
  BTree& operator=(const BTree& other) {
    // Todo Assignment
  }
  ~BTree() {
  	if(bt) fclose(bt);
    // Todo Destructor
  }
  // Insert: Insert certain Key-Value into the database
  // Return a pair, the first of the pair is the iterator point to the new
  // element, the second of the pair is Success if it is successfully inserted
  pair<iterator, OperationResult> insert(const Key& key, const Value& value) {
  	iterator nothing;
  	if(empty()){
  		node_y first;
  		first.pos=mation->end;
  		first.size=1;
  		first.key[0]=key;
  		first.val[0]=value;
  		mation->end+=sizeof(node_y);
  		mation->root=mation->head=mation->tail=first.pos;
		fseek(bt,first.pos,0);
		fwrite(&first,sizeof(node_y),1,bt);
	}
	else if(mation->size<=l){
		node_y first;
		fseek(bt,sizeof(info),0);
		fread(&first,sizeof(node_y),1,bt);
		int i; 
		for(i=0;i<first.size&&first.key[i]<key;i++);
		if(first.key[i]==key) return pair<iterator,OperationResult>(nothing,Fail);
		for(int j=first.size-1;j>=i;j--){
			first.key[j+1]=first.key[j];
			first.val[j+1]=first.val[j];
		}
		first.key[i]=key;
		first.val[i]=value;
		first.size++;
		if(first.size==l+1) crash_leaf(first);
		fseek(bt,first.pos,0);
		fwrite(&first,sizeof(node_y),1,bt);
	}  
	else {
		pair<ssize_t,int> pos=locate(key,mation->root);
		node_y land;
		fseek(bt,pos.first,0);
		fread(&land,sizeof(node_y),1,bt);
		if(land.key[pos.second+1]==key) return pair<iterator,OperationResult>(nothing,Fail);
		for(int i=land.size-1;i>pos.second;i--){
			land.key[i+1]=land.key[i];
			land.val[i+1]=land.val[i];
		}
		land.key[pos.second+1]=key;
		land.val[pos.second+1]=value;
		land.size++;
		if(pos.second==-1) {
			node fat;
			fseek(bt,land.fa,0);
			fread(&fat,sizeof(node),1,bt);
			int i;
			for(i=1;i<fat.size&&fat.key[i]<land.key[1];i++);
			if(fat.key[i]==land.key[1]) fat.key[i]=land.key[0];
			else update(fat,land.key[0],land.key[1]);
			fseek(bt,fat.pos,0);
			fwrite(&fat,sizeof(node),1,bt);
		}
		if(land.size>l) crash_leaf(land);
		fseek(bt,land.pos,0);
		fwrite(&land,sizeof(node_y),1,bt);
	}
	mation->size++;
	fseek(bt,0,0);
	fwrite(mation,sizeof(info),1,bt);
    // TODO insert function
  }
  
  // Erase: Erase the Key-Value
  // Return Success if it is successfully erased
  // Return Fail if the key doesn't exist in the database
  OperationResult erase(const Key& key) {
    // TODO erase function
    return Fail;  // If you can't finish erase part, just remaining here.
  }
  // Overloaded of []
  // Access Specified Element
  // return a reference to the first value that is mapped to a key equivalent to
  // key. Perform an insertion if such key does not exist.
  Value& operator[](const Key& key) {}
  // Overloaded of const []
  // Access Specified Element
  // return a reference to the first value that is mapped to a key equivalent to
  // key. Throw an exception if the key does not exist.
  const Value& operator[](const Key& key) const {}
  // Access Specified Element
  // return a reference to the first value that is mapped to a key equivalent to
  // key. Throw an exception if the key does not exist
  Value& at(const Key& key) {
  	if(mation->size<=l){
  		node_y aim;
  		fseek(bt,mation->head,0);
		fread(&aim,sizeof(node_y),1,bt);
		for(int i=0;i<aim.size;i++){
			if(aim.key[i]==key) return aim.val[i];
		}
	  }
  	pair<ssize_t,size_t> find=locate(key,mation->root);
	node_y aim;
	fseek(bt,find.first,0);
	fread(&aim,sizeof(node_y),1,bt);
	return aim.val[find.second+1];
  }
  // Overloaded of const []
  // Access Specified Element
  // return a reference to the first value that is mapped to a key equivalent to
  // key. Throw an exception if the key does not exist.
  const Value& at(const Key& key) const {
		return at(key);
  }
  // Return a iterator to the beginning
  iterator begin() {}
  const_iterator cbegin() const {}
  // Return a iterator to the end(the next element after the last)
  iterator end() {}
  const_iterator cend() const {}
  // Check whether this BTree is empty
  bool empty() const {
  	if(mation->size==0) return true;
	  else return 0; 
  }
  // Return the number of <K,V> pairs
  size_t size() const {return mation->size;}
  // Clear the BTree
  void clear() {
  		mation->size=mation->root=mation->head=mation->tail=0;
  		mation->end=sizeof(info);
  		fseek(bt,0,0);
  		fwrite(mation,sizeof(info),1,bt);
  		return;
  }
  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key& key) const {}
  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is
   * returned.
   */
  iterator find(const Key& key) {}
  const_iterator find(const Key& key) const {}
};
}  // namespace sjtu
