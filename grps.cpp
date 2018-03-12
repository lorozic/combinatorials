#include <iostream>
#include <cstdlib>
#include <fstream>

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
  // TODO: need checks to see if it is in fact a group?
  // TODO: compute closures? -- done with simple, not SS
public:
  set<Elem> m_set;

  void add(Elem e) {
    m_set.insert(e);
  }

  void extend(Elem e) {
    // NOTE: this creates the closure of the group immediately
    Elem id = e * -e;

    // TODO: umjesto pretvorbe, jednostavno sve raditi prek iteratora
    vector<Elem> generators(m_set.begin(), m_set.end());
    generators.push_back(e);
    m_set.clear();

    *this = Group<Elem>::simple_gen(id, generators);
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

  }

  vector<Group<Elem>> disjunctUnion() {
    // TODO: napisati particiju pomocu cosetova
  };
  //Elem operator+(Elem a, Elem b) = 0;

  friend ostream& operator<<(ostream& os, const Group<Elem> g) {
    typename set<Elem>::iterator it = g.m_set.begin();
    int i = 0;
    while(it != g.m_set.end()) {
      Elem inv = *it;
      inv = -inv;
      os << i << ". " << (*it) << " -- " << inv << endl;
      it++;
      i++;
    }
    return os;
  }

}; /////////////////////////////////////// END Group

////////////////////////////////////////////////////////////////////////////////
// CLASS: Permutation
// Reprezentira genericku permutaciju (permutaciju skupa 1 .. n)
// Definira osnovne operacije: TODO
////////////////////////////////////////////////////////////////////////////////
class Permutation {
public:
  vector<int> m_perm;

public:
  bool test_next() {
    return next_permutation(m_perm.begin(), m_perm.end());
  }

  bool next() {
    // trebam izgenerirati sljedecu permutaciju iz postojece
    // 1) nadjem prvi zdesna koji ne postuje poredak
    int i;
    int idx1 = -1;
    int idx2 = -1;
    for (i = size() - 2; i >= 0; --i ) {
      if (m_perm[i+1] > m_perm[i]) {
        // daljnji mora biti veci, ako nije nasli smo
        idx1 = i;
        break;
      }
    }

    // ako smo sve prosli i nismo jos nasli indeks, onda smo na zadnjoj permutaciji, nema sljedece
    // eventualno, mogu ici u krug ali trenutno tu hocu stati
    if (idx1 == -1)
      return false;

    // sad nadjemo zadnji element veci od perm[i] (tj. prvi zdesna)
    for (i = size() - 1; i >= 0; --i ) {
      if (m_perm[i] > m_perm[idx1]) {
        idx2 = i;
        break;
      }
    }

    // ovaj uvijek mora postojati al svejedno checkiram
    if (idx2 == -1) {
      cerr << "Ovo se ne smije dogoditi, " << __PRETTY_FUNCTION__ << endl;
      exit(1);
    }

    // sad zamijenimo ta dva indeksa
    std::swap(m_perm[idx1], m_perm[idx2]);
    // i okrenemo listu
    std::reverse(m_perm.begin()+idx1+1, m_perm.end());

    return true;
  }

  void simple_ciclify() {
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


// NOTE: fja testiranja pripadnosti grupi
// TODO: ovo organizirati na nacin da
// se moze pozvati samo za grupe nad permutacijama, nema smisla raditi
// genericki
int test(Permutation &g, vector<vector<Permutation>> SchreierVector) {
  int i;
  // TODO: provjera dal su vektori odgovarajucih velicina!
  Permutation f(g.size());

  for (i = 0; i < g.size(); ++i) {
    int x = g(i);

    // sad trazim h iz Ui, t.d. h(i) = x
    // gledam i-ti schreier vector i trazim h unutra
    int jidx;
    for (jidx = 0; jidx < SchreierVector[i].size(); ++jidx) {
      const Permutation &h = SchreierVector[i][jidx];
      ////cout << "H: ";
      ////cout << h << endl;
      if (h(i) == x)
        break;
    }

    // NOTE: kad izadjemo, ili je j == SchreierVector[i].size(), ili imam j
    // koji trebam
    if (jidx == SchreierVector[i].size()) {
      // nismo pronasli takav h
      return i;
    }

    // tu imamo takav h pa gledam njegov inverz
    Permutation hinv = -SchreierVector[i][jidx];

    cout << "$$$$$ Step: i=" << i << " jidx=" << jidx << endl;
    cout << "$$$$$ H: ";
    cout << -hinv << endl;

    // sad mnozimo g slijeva 
    f = f * (-hinv);
    g = hinv * g;
    cout << "$$$$$ New g: ";
    cout << g << endl;

    cout << "$$$$$ New f: ";
    cout << f << endl;
  }

  return g.size();
}

// TODO: procedura za ekstenzije Schreiera
// NOTE: prosljedjujem referencu da bi se uredno mijenjala po potrebi
void enter(Permutation &g, vector<vector<Permutation>> &SV) {
  int i = test(g, SV);
  cout << "#### enter begins #### " << endl;
  cout << "TESTING: " << g << endl;
  cout << "i = " << i << endl;
  int n = g.size();

  // TODO: ovaj n bi trebal biti dio grupe permutacija, makar ne znam kolko je
  // to izvedivo

  // ako je vec unutra, onda ne trebamo prosiriti
  if (i == n) {
    cout << "exiting ..." << endl;
    return;
  }

  // inace, prosirujemo i-ti
  cout << "pushing back " << g << endl;
  SV[i].push_back(g);
  cout << "#######################################################"<< endl;
  for(int i = 0; i < SV.size(); ++i) {
    cout << "################" << endl;
    cout << "U" << i << endl;
    for(int j = 0; j < SV[i].size(); ++j) {
      cout << SV[i][j] << endl;
    }
  }
  cout << "#######################################################"<< endl;


  // sad kad je u Ui, trebamo napraviti zatvorenje
  // to radimo tak da za svaki daljnji Uj izmnozimo s g zdesna?
  for (int j = i; j < SV.size(); ++j) {
    cout << "gledam U" << j << endl;
    for (int hidx = 0; hidx < SV[j].size(); ++hidx) {
      Permutation f = SV[j][hidx] * g;
      cout << "Uzimam h=" << SV[j][hidx] << endl;
      cout << "hidx=" << hidx << endl;
      cout << "Izracunah f, f = " << f << endl;
      enter(f, SV);
    }
  }
}

int main(void) {
  //Group<Permutation> g;
  //Permutation a(8);
  //Permutation b(a.size());
  // definiram SS reprezentaciju, hocu vrtiti SS algoritam po njoj
  // TODO: na zalost ovo treba hardkodirati dok ne napisem sve ostalo
  vector<Permutation> u0, u1, u2, u3, u4, u5, u6, u7;
  Permutation I(8);
  Permutation t(8);

  u0.push_back(I);
  t[0] = 1; t[1] = 3; t[3] = 7; t[7] = 6; t[6] = 4; t[4] = 0;
  t[2] = 5; t[5] = 2;
  u0.push_back(t);
  t[0] = 2; t[2] = 6; t[6] = 4; t[4] = 0;
  t[1] = 3; t[3] = 7; t[7] = 5; t[5] = 1;
  u0.push_back(t);
  t[0] = 3; t[3] = 6; t[6] = 0;
  t[1] = 7; t[7] = 4; t[4] = 1;
  t[2] = 2;
  t[5] = 5;
  u0.push_back(t);
  t[0] = 4; t[4] = 6; t[6] = 7; t[7] = 3; t[3] = 1; t[1] = 0;
  t[2] = 5; t[5] = 2;
  u0.push_back(t);
  t[0] = 5; t[5] = 3; t[3] = 6; t[6] = 0;
  t[1] = 7; t[7] = 2; t[2] = 4; t[4] = 1;
  u0.push_back(t);
  t[0] = 6; t[6] = 3; t[3] = 0;
  t[1] = 4; t[4] = 7; t[7] = 1;
  t[2] = 2;
  t[5] = 5;
  u0.push_back(t);
  t[0] = 7; t[7] = 0;
  t[1] = 6; t[6] = 1;
  t[2] = 5; t[5] = 2;
  t[3] = 4; t[4] = 3;
  u0.push_back(t);

  u1.push_back(I);
  t[0] = 0;
  t[1] = 2; t[2] = 1;
  t[3] = 3;
  t[4] = 4;
  t[5] = 6; t[6] = 5;
  t[7] = 7;
  u1.push_back(t);
  t[0] = 0;
  t[1] = 4; t[4] = 2; t[2] = 1;
  t[3] = 5; t[5] = 6; t[6] = 3;
  t[7] = 7;
  u1.push_back(t);

  u2.push_back(I);
  u3.push_back(I);
  u4.push_back(I);
  u5.push_back(I);
  u6.push_back(I);
  u7.push_back(I);
  vector<vector<Permutation>> SSvec;
  SSvec.push_back(u0);
  SSvec.push_back(u1);
  SSvec.push_back(u2);
  SSvec.push_back(u3);
  SSvec.push_back(u4);
  SSvec.push_back(u5);
  SSvec.push_back(u6);
  SSvec.push_back(u7);

  // po svoj logici sad valjda imam ss reprezentaciju
  // sad trebam testirati dal je g unutra
  Permutation g1(8);
  g1[0] = 6; g1[6] = 0;
  g1[1] = 7; g1[7] = 1;
  g1[2] = 4; g1[4] = 2;
  g1[3] = 5; g1[5] = 3;

  Permutation g2(8);
  g2[0] = 1; g2[1] = 2; g2[2] = 3; g2[3] = 4; g2[4] = 0;
  g2[5] = 6; g2[6] = 7; g2[7] = 5;


  cout << "Test result: " << test(g1, SSvec) << endl;
  cout << "Test result: " << test(g2, SSvec) << endl;

  Permutation alpha(8), beta(8);
  alpha[0] = 1; alpha[1] = 3; alpha[3] = 7;
  alpha[7] = 6; alpha[6] = 4; alpha[4] = 0;
  alpha[2] = 5; alpha[5] = 2;

  beta[0] = 1; beta[1] = 3; beta[3] = 2; beta[2] = 0;
  beta[4] = 5; beta[5] = 7; beta[7] = 6; beta[6] = 4;

  // imam alfa i beta (generatore), generiram SS
  vector<vector<Permutation>> SS;
  vector<Permutation> idgrp;

  // inicijaliziram SS vektor
  idgrp.push_back(Permutation(8));
  SS.push_back(idgrp);
  SS.push_back(idgrp);
  SS.push_back(idgrp);
  SS.push_back(idgrp);
  SS.push_back(idgrp);
  SS.push_back(idgrp);
  SS.push_back(idgrp);
  SS.push_back(idgrp);

  enter(alpha, SS);
  cout << "1234 gotov prvi" << endl;
  enter(beta, SS);

  for(int i = 0; i < SS.size(); ++i) {
    cout << "################" << endl;
    cout << "U" << i << endl;
    for(int j = 0; j < SS[i].size(); ++j) {
      cout << SS[i][j] << endl;
    }
  }

  // neki primjeri iz knjige
  /*a[0] = 1; a[1] = 3; a[2] = 5; a[3] = 7;
  a[4] = 0; a[5] = 2; a[6] = 4; a[7] = 6;

  b[0] = 1; b[1] = 3; b[2] = 0; b[3] = 2;
  b[4] = 5; b[5] = 7; b[6] = 4; b[7] = 6;
  
  //b.simple_ciclify(); 
  g.extend(a);
  g.extend(b);

  //ofstream ff("automorfizmi");
  cout << g << endl;
  */

  //Group<Permutation> h;
  //h.generate_full_group(Permutation(a.size()));
  //cout << h << endl;
  /*
  while (1) {
    a.my_next();
    b.next();
    cout << a << endl;
    cout << b << endl;
    string in;
    cin >> in;
    if (in == "q")
      break;
      }*/


  //cout << a << endl;
  //cout << b << endl;
  //cout << (a*b) << endl;
  //h.add(a);
  //h.add(b);
  //h.add(a*b);

  //Permutation c(5);
  //vector<Permutation> generators;
  //generators.push_back(a);

  //Group<Permutation> g;
  //g = Group<Permutation>::simple_gen(Permutation(5), generators);
  //g.generate_full_group(c);
  //cout << h << endl;
  //cout << g << endl;

  //c.next();
  //c.ciclify();
  //cout << g.left_coset(c) << endl;

  return 0;
}
