#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class MacroProcessor {
public:

  void Pass1(ifstream &fin, ofstream &fout, ofstream &mnt, ofstream &pnt,
             ofstream &kpd, ofstream &pass1_op) {
    map<string, int> pntab;
    int index, pp, kp, mdtp, kpdtp, pn, j, loc;
    string name, str1, str;
    vector<string> v;
    bool profile = false;
    bool default_check = false;
    bool inside_macro = false;
    index = 0;
    mdtp = 1;
    kpdtp = 1;
    loc = 0;

    while (getline(fin, str)) {
      
      j = 0;
      str1 = str;
      for (int i = 0; i < str.length(); i++) {
        if (str[i] == ' ') {
          v.push_back(str.substr(j, i - j));
          j = i + 1;
        }
      }
      v.push_back(str.substr(j, str.length() - j));
      for (int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
      }
      cout << endl;
      if (v[0] == "MACRO") {
        index++;
        pn = 0;
        pp = 0;
        kp = 0;
        profile = true;
        inside_macro = true;
      } else if (v[0] == "MEND") {
        pntab.clear();

        loc++;
        fout << loc << " MEND" << endl;
        if (kp == 0)
          mnt << index << " " << name << " " << pp << " " << kp << " " << mdtp
              << " "
              << "-" << endl;
        else
          mnt << index << " " << name << " " << pp << " " << kp << " " << mdtp
              << " " << kpdtp << endl;
        mdtp = loc + 1;
        kpdtp += kp;
        inside_macro = false;
      } else {
        if (profile && inside_macro) {
          name = v[0];
          pn = 0;
          for (int i = 1; i < v.size(); i++) {
            if (v[i][v[i].length() - 1] == ',') {
              v[i] = v[i].substr(0, v[i].length() - 1);
            }
            if (v[i][0] == '&') {

              if (v[i][v[i].length() - 1] == '=') {
                kp++;
                v[i] = v[i].substr(1, v[i].size() - 2);
                if (i != v.size() - 1 && v[i + 1][0] != '&')
                  default_check = true;
                if (!default_check)
                  kpd << /*kpdtp+kp-1<<" "<<*/ v[i] << " -" << endl;
                else
                  kpd << /*kpdtp+kp-1<<" "<<*/ v[i] << " ";
                default_check = false;

              } else {
                pp++;
                v[i] = v[i].substr(1, v[i].length());
              }

              pn++;
              pntab.insert({v[i], pn});
              pnt << pn << " " << v[i] << endl;
            } else {
              kpd << v[i] << endl;
            }
          }
          profile = false;
        } else if (!profile && inside_macro) {
          loc++;
          fout << loc << " " << v[0] << " ";
          for (int i = 1; i < v.size(); i++) {

            if (v[i][v[i].length() - 1] == ',')
              v[i] = v[i].substr(0, v[i].length() - 1);
            if (v[i][0] == '&') {
              v[i] = v[i].substr(1, v[i].length() - 1);
              fout << "(P," << pntab[v[i]] << ")";
            } else
              fout << v[i];
            if (i != v.size() - 1)
              fout << ", ";
            else
              fout << endl;
          }
        } else {
          pass1_op << str << endl;
        }
      }
      v.clear();
    }
    fin.close();
    fout.close();
    pnt.close();
    mnt.close();
    kpd.close();
  }
};
int main() {
  MacroProcessor P;
  ifstream fin;
  ofstream fout, mnt, pnt, kpd, pass1_op;
  fin.open("sample.txt");
  fout.open("mdt.txt");
  mnt.open("mnt.txt");
  pnt.open("pnt.txt");
  kpd.open("kpd.txt");
  pass1_op.open("pass1_op.txt");
  P.Pass1(fin, fout, mnt, pnt, kpd, pass1_op);

  return 0;
}