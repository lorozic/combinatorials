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

typedef vector<vector<Permutation>> SVec;

////////////////////////////////////////////////////////////////////////////////
// Klasa: Group
// Reprezentira genericku ciklicku grupu. Definira neke osnovne operacije.
////////////////////////////////////////////////////////////////////////////////
template <class Elem>
class Group {
public:
  set<Elem> m_set;

  Group () { }
  Group (Elem e) { m_set.insert(e); }
  Group(set<Elem> init) {
    m_set = init;
  }

  void add(Elem e) {
    m_set.insert(e);
  }

  virtual void extend(Elem e) {
    // NOTE: this creates the closure of the group immediately
    Elem id = e * -e;

    vector<Elem> generators(m_set.begin(), m_set.end());
    generators.push_back(e);
    m_set.clear();

    *this = Group<Elem>::simple_gen(id, generators);
  }

  static Group<Elem> simple_gen(Elem id, vector<Elem> generators) {
    // quick and dirty
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

  friend bool operator==(const Group<Elem> &g1, const Group<Elem> &g2) {
    return g1.m_set == g2.m_set;
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

  Elem operator[](size_t idx) {
    if (idx >= m_set.size()) {
      cerr << "Invalid index (operator[])!" << endl;
      exit(1);
    }

    typename set<Elem>::iterator it = m_set.begin();
    while(idx != 0) {
      idx--;
      it++;
    }

    return *it;
  }

}; /////////////////////////////////////// END Group

////////////////////////////////////////////////////////////////////////////////
// Klasa: Permutation
// Reprezentira genericku permutaciju (permutaciju skupa 1 .. n)
// Definira osnovne operacije nad permutacijama
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
    // nadjem prvi zdesna koji ne postuje poredak
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

    // ako smo sve prosli i nismo jos nasli indeks, onda smo na zadnjoj
    // permutaciji, nema sljedece eventualno, mogu ici u krug ali trenutno tu
    // hocu stati
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

  int& operator[](size_t i) {
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
    int temp1 = (*this)[i];
    int temp2 = (*this)[j];
    (*this)[j] = temp1;
    (*this)[i] = temp2;
  }

  size_t size() const {
    return m_perm.size();
  }

  Permutation() {
    // NOTE: ovo je loooose za koristiti
  }

  Permutation(size_t size) {
    // napravimo identitet
    m_perm.resize(size, 0);
    for (size_t i = 0; i < size; ++i) {
      m_perm[i] = i;
    }
  }

  Permutation(vector<int> init) : m_perm(init)
    { /* */ }

  // NOTE: primam kopiju permutacije da ju mogu mijenjati dolje
  friend ostream& operator<<(ostream& of, const Permutation &p) {
    int fst = -1;
    int i = 0;
    vector<bool> mask(p.size(), false);
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

////////////////////////////////////////////////////////////////////////////////
/////////////////////// UTIL FUNKCIJE //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Permutation readPerm() {
  cout << "Paznja -- fja ne pazi na krive unose!" << endl;
  cout << "Velicina: ";

  int sz;
  cin >> sz;
  vector<int> perm(sz);
  int cicles;
  int elems;

  cout << "Broj ciklusa: ";
  cin >> cicles;
  for(int i = 0; i < cicles; ++i) {
    cout << "Broj elemenata u ciklusu " << i << ": ";
    cin >> elems;

    int first;
    int last;
    cin >> first;
    last = first;
    for (int j = 1; j < elems; ++j) {
      int el;
      cin >> el;
      perm[last] = el;
      last = el;
    }
    perm[last] = first;
  }

  return Permutation(perm);
}

void decomposition_h(Permutation g, vector<vector<Permutation>> SchreierVector, vector<Permutation> &decomp) {
  int i;
  for (i = 0; i < g.size(); ++i) {
    int x = g(i);
    int jidx;
    for (jidx = 0; jidx < SchreierVector[i].size(); ++jidx) {
      const Permutation &h = SchreierVector[i][jidx];
      if (h(i) == x)
        break;
    }

    if (jidx == SchreierVector[i].size()) {
      return;
    }

    Permutation hinv = -SchreierVector[i][jidx];

    if (-hinv != (-hinv)*hinv)
      decomp.push_back(-hinv);

    // NOTE: sad pamtim svaki od tih H-ova i ne mijenjam originalnu perm
    g = hinv * g;
  }
}

// F: fja testiranja pripadnosti grupi
// TODO: nek ne mijenja g nego mu damo da pamti f
int test(Permutation &g, vector<vector<Permutation>> SchreierVector) {
  int i;
  Permutation f(g.size());

  for (i = 0; i < g.size(); ++i) {
    int x = g(i);

    // sad trazim h iz Ui, t.d. h(i) = x
    // gledam i-ti schreier vector i trazim h unutra
    int jidx;
    for (jidx = 0; jidx < SchreierVector[i].size(); ++jidx) {
      const Permutation &h = SchreierVector[i][jidx];
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

    // sad mnozimo g slijeva 
    //f = f * (-hinv);
    g = hinv * g;
  }
  return g.size();
}

// NOTE: prosljedjujem referencu da bi se uredno mijenjala po potrebi
void enter(Permutation &g, vector<vector<Permutation>> &SV) {
  int i = test(g, SV);
  int n = g.size();

  // ako je vec unutra, onda ne trebamo prosiriti
  if (i == n) {
    return;
  }

  // inace, prosirujemo i-ti
  SV[i].push_back(g);

  // sad kad je u Ui, trebamo napraviti zatvorenje
  // to radimo tak da za svaki daljnji Uj izmnozimo s g zdesna?
  for (int j = 0; j <= i; ++j) {
    for (int hidx = 0; hidx < SV[j].size(); ++hidx) {
      Permutation f = g * SV[j][hidx];
      enter(f, SV);
    }
  }
}

// NOTE: primam elemSet ko referencu, tak da se u njega mogu dodati elementi 
void backtrack(int n, int l, vector<vector<Permutation>> SV, Permutation g, set<Permutation> &elemSet) {
  if (n == l) {
    elemSet.insert(g);
    return;
  }

  // else
  for (int i = 0; i < SV[l].size(); ++i) {
    const Permutation& h = SV[l][i]; // iscupam kojeg trebam
    backtrack(n, l+1, SV, g * h, elemSet);
  }
}

// NOTE: procedura za generiranje kompletne grupe iz SS zapisa
// TODO: ovo je glupo ime, trebam bolje -- ovo bi trebalo primiti identitet na mjesto g
Group<Permutation> quick_generate(vector<vector<Permutation>> SV, Permutation ID) {
  int l = 0;
  set<Permutation> elemSet;
  backtrack(ID.size(), l, SV, ID, elemSet); // vracamo novu grupu

  return Group<Permutation>(elemSet);
}

vector<Permutation> decomposition(Permutation g, vector<vector<Permutation>> SchreierVector) {
  vector<Permutation> decomp;
  decomposition_h(g, SchreierVector, decomp);
  return decomp;
}

void printSV(SVec SV) {
  for(int i = 0; i < SV.size(); ++i) {
    cout << "U" << i << ": " << endl;
    for(int j = 0; j < SV[i].size(); ++j) {
      cout << SV[i][j] << endl;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Klasa: PermGroup
// Reprezentira grupu permutacija. Cuva SS reprezentaciju grupe.
////////////////////////////////////////////////////////////////////////////////
class PermGroup : public Group<Permutation> {
  vector<vector<Permutation>> m_SV;
  int m_perm_size;
  Permutation m_ID;

public:
  // F: radi grupu permutacija koja se sastoji samo od identiteta
  PermGroup(int perm_size) : Group<Permutation>() {
    m_set.insert(Permutation(perm_size));
    m_perm_size = perm_size;

    m_ID = Permutation(m_perm_size);
    vector<Permutation> ID_vec;

    // kreiram vektor identiteta
    ID_vec.push_back(m_ID);

    // taj vektor koristim za inicijalizaciju SS-a
    for (int i = 0; i < m_perm_size; ++i) {
      m_SV.push_back(ID_vec);
    }
  }

  // F: za dani skup generatora, generira kompletnu grupu
  PermGroup(int perm_size, set<Permutation> generators) : PermGroup(perm_size) {
    // NOTE preduvjet je da su generatori svi iste velicine -- ne provjeravam!

    // prvo izgeneriram m_SV
    set<Permutation>::iterator it = generators.begin();
    while(it != generators.end()) {
      // TODO: kad rijesim to da enter mijenja ulaznu permutaciju, trebam
      // sloziti i tu
      Permutation temp = *it;
      enter(temp, m_SV);
      it++;
    }

    // onda izgeneriram sve elemente iz SV reprezentacije
    backtrack(m_ID.size(), 0, m_SV, m_ID, m_set); // vracamo novu grupu
  }

  bool testSV(Permutation p) {
    return test(p, m_SV) == m_perm_size;
  }

  // vraca dekompoziciju dane permutacije u odnosu na SV 
  vector<Permutation> decompose(Permutation p) {
    vector<Permutation> vec;

    Permutation temp = p; // iskopiram p
    if (!testSV(p))
      return vec; // ako nije u grupi vracamo prazni vektor

    return decomposition(p, m_SV);
  }

  vector<vector<Permutation>> getSV () {
    return m_SV;
  }

  // F: izgenerira najvecu grupu danog reda
  void generate_full_group() {
    // pobrisemo sve elemente
    m_set.clear();
    // ubacimo prvi
    m_set.insert(m_ID);

    // ubacimo ostale
    while(m_ID.next()) {
      m_set.insert(m_ID);
    }
  }
};


////////////////////////////////////////////////////////////////////////////////
int main(void) {

  Permutation alpha(8), beta(8);
  //Permutation alpha_save(8), beta_save(8);
  alpha[0] = 1; alpha[1] = 3; alpha[3] = 7;
  alpha[7] = 6; alpha[6] = 4; alpha[4] = 0;
  alpha[2] = 5; alpha[5] = 2;

  beta[0] = 1; beta[1] = 3; beta[3] = 2; beta[2] = 0;
  beta[4] = 5; beta[5] = 7; beta[7] = 6; beta[6] = 4;

  set<Permutation> generatori;
  generatori.insert(alpha);
  generatori.insert(beta);
  PermGroup G(8, generatori);
  cout << G << endl;

  cout << alpha << endl;
  cout << beta << endl;

  Permutation elem = G[25];
  cout << elem << endl;

  cout << "Dekompozicija permutacije " << elem << ": " << endl;
  vector<Permutation> decomp = G.decompose(elem);
  for (int i = 0; i < decomp.size(); ++i) {
    cout << decomp[i] << endl;
  }

  printSV(G.getSV());

  return 0;
}
