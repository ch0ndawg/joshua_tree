//
//  joshua_tree.cpp
//  
//
//  Created by Chris on 12/19/15.
//
//

#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

struct jt_node {
    int val;
    jt_node *next;
    jt_node *super_next;
    jt_node(int v=0) : val(v), next(nullptr), super_next(nullptr) {}
};

class joshua_tree {
    jt_node *head;
    jt_node *tail;
    int sz;
    int sqrt_sz;
public:
    joshua_tree() : head(new jt_node), sz(0), sqrt_sz(0) {}
    joshua_tree(istream &is, int N);
    jt_node* node_at(int);
    int node_group_at(jt_node &, int);
    int &operator [] (int i) {return node_at(i)->val;}
  //  void insert(int data);
    void print(ostream &) const;
    int size() const {return sz ; }
    void query_1(int i, int j);
    void query_2(int i, int j);
    
    ~joshua_tree() { jt_node* curr =head;  while(curr) { jt_node* oldNext = curr->next; delete curr; curr=oldNext;} delete tail;}
};

inline ostream &operator<<(ostream&os, const joshua_tree& jt)
{
    jt.print(os);
    return os;
}

//void joshua_tree::insert(int data)
//{
//    jt_node oldTail = tail->next;
//    tail->next = new jt_node(data);
//    oldTail->next = tail->next;
//    
//    if ( sqrt(++sz) >= sqrt_sz+1) {
//        sqrt_sz++;
//        
//        jt_node *curr = head->next;
//        jt_node *prev = head->next;
//        
//        
//        while (curr) {
//            prev = curr;
//            curr = curr->super_next;
//            prev->super_next = curr->next; // update the super next's to be all
//        }
//        curr = prev;
//        while (curr) {
//            prev = curr;
//            curr = curr->next;
//        }
//    }
//}

int joshua_tree::node_group_at(jt_node& result, int i)
{
    jt_node *curr = head->next, *prev = head;
    int k;
    for (k=sqrt_sz; k<= i; k+= sqrt_sz) {
        prev = curr;
        curr = curr->super_next; // skip
    }
    result.next = curr; // hack
    result.super_next = prev;
    return k-sqrt_sz;
}

jt_node *joshua_tree::node_at(int i)
{
    jt_node result;
    int k = node_group_at(result,i);
    jt_node *curr = result.next;
    
    for (; k < i; k++) {
       // prev=curr;
        curr = curr->next; // direct next
    }
    return curr;
}


void joshua_tree::query_1(int i, int j)
{
    if (i==0) return; // do nothing on move initial to initial
    
    int m = j-i+1; // number of elements to move
    
    jt_node res;
    int k= node_group_at(res,i);
    jt_node *psg = res.super_next; // previous start group
    bool firstGroup = (psg==head);
    jt_node *curr=res.next;
    
    jt_node *beforeFirst = (i % sqrt_sz ? curr : node_at(i-1));
    
    
    
    for (; k < i; k++) {
        // prev=curr;
        beforeFirst = curr;
        psg = psg->next;
        curr = curr->next; // direct next
    }
    
    //assert (psg->super_next == curr);
    jt_node *start = curr; // first node to be moved
    
    k = node_group_at(res,j);
    jt_node *peg = res.super_next; // previous end group
    jt_node *last=res.next; // end of the range
    jt_node *ep = head;
    
    for (; k < j; k++) {
        // prev=curr;
        ep = last;
        peg = peg->next;
        last = last->next; // direct next
    }

    jt_node* firstPast = last->next;
    
    beforeFirst->next = firstPast; // collapse all the nodes between sp and last
    
    
    // DONE omitting the range
    
    // save old first node
    jt_node *oldFirst = head->next;
    // set head to point to the moved start node
    head->next = head->super_next = start;
    // set old first node to follow last
    last->next = oldFirst;
    
    jt_node *sp = firstPast;
    
    // now must clean up the "super" links
    if (firstGroup) {
        psg = oldFirst;
        for (int ell = i; ell < sqrt_sz; ell++) // advance it until equal
                if(sp) sp = sp->next;
    }
    
    while (psg != firstPast) {
        psg->super_next = sp;
        psg = psg->next;
        if (sp) sp = sp->next; // null out any of them at the last
    }
    
    // now fix the nodes in the last sqrt_sz elements of the moved nodes
    
    peg = peg->next;
    curr = oldFirst;
    
    if (m<= sqrt_sz) { // if it is smaller than...
        peg = start;
        for (int ell=m; ell <sqrt_sz; ell++) {
            if (curr) curr = curr->next;
        }
    }
    
    while  (peg != oldFirst) {
        peg->super_next = curr;
        peg = peg->next;
        if (curr) curr = curr->next;
    }
    
    if (j == sz-1) { // if range included last node, make it the new tail
        tail->next = beforeFirst;
    }
}


void joshua_tree::query_2(int i, int j)
{
    if (j==sz-1) return; // do nothing on move final to final
    
    int m = j-i+1; // number of elements to move
    
    jt_node res;
    int k= node_group_at(res,i);
    jt_node *psg = res.super_next; // previous start group
    bool firstGroup = (psg==head);
    jt_node *curr=res.next;
    
    jt_node *beforeFirst = (i % sqrt_sz ? curr : node_at(i-1));
    
    
    
    for (; k < i; k++) {
        // prev=curr;
        beforeFirst = curr;
        psg = psg->next;
        curr = curr->next; // direct next
    }
    
    //assert (psg->super_next == curr);
    jt_node *start = curr; // first node to be moved
    
    k = node_group_at(res,j);
    jt_node *peg = res.super_next; // previous end group
    jt_node *last=res.next; // end of the range
    jt_node *ep = head;
    
    for (; k < j; k++) {
        // prev=curr;
        ep = last;
        peg = peg->next;
        last = last->next; // direct next
    }
    
    jt_node* firstPast = last->next;
    
    beforeFirst->next = firstPast; // collapse all the nodes between sp and last
   
    jt_node *oldTail = tail->next;
    tail->next = last; // set the end of the rage to the end of the whole list
    
    oldTail->next = first;
   
}


joshua_tree::joshua_tree(istream &is, int N)
: sz(N), sqrt_sz(static_cast<int>(sqrt(N))), head(new jt_node), tail(new jt_node)
{
    int x;
    jt_node *curr = head;
    int k;
    for (k=0; k<sqrt_sz;k++) {
        is >> x;
        curr->next = new jt_node(x);
        curr = curr->next;
    }
    
    jt_node *super_curr = head->next;
    head->super_next = head->next;
    for (k= sqrt_sz; k < sz; k++) {
        is >> x;
        curr->next = new jt_node(x);
        curr = curr->next;
        super_curr->super_next = curr; // have the super next's point to the beginning of the next block
        super_curr = super_curr->next;
    }
    
    tail->next = curr;
    tail->super_next = super_curr; // WARNING: this points oppositely!!
}


void joshua_tree::print(ostream &os) const
{
    jt_node *curr = head->next;
    for (int i=0; i<sz; i++) {
        os << curr->val << ' ' ; //<< '(' << curr << ", " << curr->next << ", " << curr->super_next << ")\n" ;
        curr = curr->next;
    }
}

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    // brute force:

    int N=0, M=0;
    cin >> N >> M;
    //M=3;
    
    joshua_tree jt(cin,N);
    
    for (int k=0; k<M; k++) {
        int choice, i,j;
        cin >> choice >> i >> j;
        if (choice == 1) {
            jt.query_1(i-1,j-1); // subtract 1 for the zero-based array indexing
        } else {
            jt.query_2(i-1,j-1);
        }
    }
    cout << abs(jt[N-1]-jt[0]) << endl;
    
 
    cout << jt << endl;

    
    return 0;
}