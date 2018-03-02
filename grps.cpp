#include <iostream>
#include <cstdlib>

#include <algorithm>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <list>

using namespace std;
class Permutation;

template <class Elem>
class Group {
  // TODO: need checks to see if it is in fac a group
  // TODO: compute closures?
public:
  set<Elem> m_set;

  //Group() : m_set(0)
  //{/*  */}

  void add(Elem e) {
    m_set.insert(e);
  }

  static Group<Elem> simple_gen(Elem id, vector<Elem> generators) {
    // TODO again, quick and dirty fuj :P
    Group<Elem> g; // prazna grupa
    set<Elem> new_elems;
    set<Elem> last;

    new_elems.insert(id);
    while (!new_elems.empty()) {
      typename set<Elem>::iterator it = new_elems.begin();
      while(it != new_elems.end()) {
        g.add(*it);
        it++;
      }

      last = new_elems;
      new_elems.clear();

      for(size_t i = 0; i < generators.size(); ++i) {
        typename set<Elem>::iterator jt = last.begin();
        while(jt != last.end()) {
          Elem f = generators[i]*(*jt);

          // provjerimo dal je vec u grupi
          if (g.m_set.count(f) == 0) {
            //cout << "generated: " << f << endl;
            new_elems.insert(f);
          }
          jt++;
        }
      }
    }
    return g;
  }

  void generate_full_group(Elem generator) {
    // NOTE klasa radi za grupe koje imaju "sljedeci" element
    // NOTE generator nije "generator grupe", nego samo "najmanji element"
    // TODO pogledati dal mogu sloziti vise genericko, al zasad mi je ovo super
    // pobrisemo sve elemente
    m_set.clear();
    // ubacimo prvi
    m_set.insert(generator);

    // ubacimo ostale
    while(generator.next()) {
      m_set.insert(generator);
    }
  }


  Group<Elem> left_coset(Elem e) const {
    Group<Elem> g;
    typename set<Elem>::iterator it = m_set.begin();
    while(it != m_set.end()) {
      g.add(e*(*it));
      it++;
    }
    return g;
  };

  vector<Group<Elem>> disjunctUnion() {
  };
  //Elem operator+(Elem a, Elem b) = 0;

  friend ostream& operator<<(ostream& os, const Group<Elem> g) {
    typename set<Elem>::iterator it = g.m_set.begin();
    while(it != g.m_set.end()) {
      os << (*it) << endl;
      it++;
    }
    return os;
  }
};

class Permutation {
public:
  vector<int> m_perm;

public:
  bool next() {
    return next_permutation(m_perm.begin(), m_perm.end());
  }

  void ciclify() {
    for(size_t i = 0; i < size(); ++i) {
      m_perm[i] = (i+1)%size();
    }
  }

  // NOTE: function does no checks!
  int& operator[](size_t i) {
    // check indices
    if (i >= size()) {
      cerr << "Index out of bounds: " << size() << " " << i << endl;
      exit(1);
    }
    return m_perm[i];
  }

  int operator()(size_t i) const {
    return m_perm[i];
  }

  void operator()(size_t i, size_t j) {
    // unlike [], this cycles the permutation instead of just changing the value
    int temp1 = (*this)[i];
    int temp2 = (*this)[j];
    (*this)[j] = temp1;
    (*this)[i] = temp2;
  }

  // TODO: find cycles needed to convert one perm into another

  size_t size() const {
    return m_perm.size();
  }

  Permutation(size_t size) {
    // napravimo identitet
    m_perm.resize(size, 0);
    // TODO ovo se moze ljepse sigurno u cpp
    for (size_t i = 0; i < size; ++i) {
      m_perm[i] = i;
    }
  }

  Permutation(vector<int> init) : m_perm(init)
    { /* */ }

  void randomize() {
    // randomizira permutaciju
    // TODO: ovo mogu i sam napraviti, bilo bi zgodno
  }

  // NOTE: primam kopiju permutacije da ju mogu mijenjati dolje
  friend ostream& operator<<(ostream& of, const Permutation &p) {
    //for (size_t i = 0; i < p.m_perm.size(); ++i) {
    //of << i << "-" << p.m_perm[i] << endl;
    //}

    int fst = -1;
    int i = 0;
    vector<bool> mask(p.size(), false);
    // TODO ovo hocu lepse
    while(1) {

      // nadjemo neiskoristeni 
      if (fst == -1) {
        for (size_t j = 0; j < p.size(); ++j) {
          if (mask[j] == false) { 
            of << "(";
            fst = j;
            i = fst;
            of << fst;
            mask[j] = true;
            break;
          }
        }
      }
       
      // ako su svi iskoristeni izadjemo
      if (fst == -1)
        break;

      // gledamo dal je trenutni == prvi
      if (p(i) == fst) {
        // dosli smo natrag do prvog
        of << ")";
        fst = -1;
      }
      else {
        of << "," << p(i);
        i = p(i);
        mask[i] = true;
      }
    }

    return of;
  }

  friend Permutation operator*(Permutation p1, const Permutation &p2) {
    return p1*=p2;
  }

  Permutation& operator*=(const Permutation p) {
    if (p.size() != size()) {
      cerr << "Sizes do not match." << endl;
      cerr << "fst: " << *this << "snd: " << p;
      cerr << "##################" << endl;
      exit(1);
    }

    vector<int> temps(p.size());
    for (size_t i = 0; i < size(); i++) {
      temps[i] = m_perm[p.m_perm[i]];
    }

    m_perm = temps;
    return *this;
  }


  // TODO: mozemo urediti permutacije, al zasad dajem neki trivijalni uvjet
  friend bool operator<(const Permutation& p1, const Permutation& p2) {
    if (p1.size() != p2.size()) {
      cerr << "Sizes do no match!" << endl;
      exit(1);
    }

    for(size_t i = 0; i < p1.size(); i++) {
      if (p1(i) < p2(i))
        return true;
      if (p1(i) > p2(i))
        return false;
    }

    return false;
  }

  Permutation& operator~() {
    // racunamo inverz "in place"
    vector<int> newPerm(size(), 0);
    for (size_t i = 0; i < size(); ++i) {
      newPerm[(*this)[i]] = i;
    }

    m_perm = newPerm;
    return *this;
  }

  Permutation operator-() {
    Permutation a(*this);
    return ~a;
  }

  friend bool operator==(const Permutation p1, const Permutation p2) {
    return p1.m_perm == p2.m_perm;
  }

  friend bool operator!=(const Permutation p1, const Permutation p2) {
    return !(p1 == p2);
  }
};

int main(void) {
  Group<Permutation> h;

  Permutation a(5);
  Permutation b(5);
  a[0] = 4; a[1] = 2; a[2] = 3; a[3] = 1; a[4] = 0; 
  //b = -a;

  //cout << a << endl;
  //cout << b << endl;
  //cout << (a*b) << endl;
  //h.add(a);
  //h.add(b);
  //h.add(a*b);

  //Permutation c(5);
  vector<Permutation> generators;
  generators.push_back(a);

  Group<Permutation> g;
  g = Group<Permutation>::simple_gen(Permutation(5), generators);
  //g.generate_full_group(c);
  //cout << h << endl;
  cout << g << endl;

  //c.next();
  //c.ciclify();
  //cout << g.left_coset(c) << endl;

  return 0;
}
