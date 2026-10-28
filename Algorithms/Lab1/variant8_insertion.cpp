#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
struct Stats { unsigned long long comparisons=0, moves=0; };
using Sort = void(*)(vector<int>&,Stats&);
void insertion(vector<int>& a,Stats& s) {
 for(size_t i=1;i<a.size();++i){int x=a[i];++s.moves;size_t j=i;
  while(j>0){++s.comparisons;if(a[j-1]<=x)break;a[j]=a[j-1];++s.moves;--j;}
  a[j]=x;++s.moves;
 }
}
void shell(vector<int>& a,Stats& s) {
 for(size_t gap=a.size()/2;gap>0;gap/=2)
  for(size_t i=gap;i<a.size();++i){int x=a[i];++s.moves;size_t j=i;
   while(j>=gap){++s.comparisons;if(a[j-gap]<=x)break;a[j]=a[j-gap];++s.moves;j-=gap;}
   a[j]=x;++s.moves;
  }
}
void selection(vector<int>& a,Stats& s) {
 for(size_t i=0;i<a.size();++i){size_t m=i;for(size_t j=i+1;j<a.size();++j){++s.comparisons;if(a[j]<a[m])m=j;}
  if(m!=i){swap(a[i],a[m]);s.moves+=3;}
 }
}
void shaker(vector<int>& a,Stats& s) {
 if(a.empty())return;size_t left=0,right=a.size()-1;
 while(left<right){bool changed=false;
  for(size_t i=left;i<right;++i){++s.comparisons;if(a[i]>a[i+1]){swap(a[i],a[i+1]);s.moves+=3;changed=true;}}
  if(!changed)break;--right;changed=false;
  for(size_t i=right;i>left;--i){++s.comparisons;if(a[i-1]>a[i]){swap(a[i-1],a[i]);s.moves+=3;changed=true;}}
  ++left;if(!changed)break;
 }
}
void binaryInsertion(vector<int>& a,Stats& s) {
 for(size_t i=1;i<a.size();++i){int x=a[i];++s.moves;size_t l=0,r=i;
  while(l<r){size_t m=l+(r-l)/2;++s.comparisons;if(a[m]<=x)l=m+1;else r=m;}
  for(size_t j=i;j>l;--j){a[j]=a[j-1];++s.moves;}a[l]=x;++s.moves;
 }
}
void mergeRange(vector<int>& a,vector<int>& tmp,size_t l,size_t r,Stats& s){
 if(r-l<2)return;size_t m=l+(r-l)/2;mergeRange(a,tmp,l,m,s);mergeRange(a,tmp,m,r,s);
 size_t i=l,j=m,k=l;
 while(i<m&&j<r){++s.comparisons;tmp[k++]=(a[i]<=a[j]?a[i++]:a[j++]);++s.moves;}
 while(i<m){tmp[k++]=a[i++];++s.moves;}while(j<r){tmp[k++]=a[j++];++s.moves;}
 for(size_t p=l;p<r;++p){a[p]=tmp[p];++s.moves;}
}
void mergeSort(vector<int>& a,Stats& s){vector<int> tmp(a.size());mergeRange(a,tmp,0,a.size(),s);}
void quickRange(vector<int>& a,int l,int r,Stats& s){
 while(l<r){int i=l,j=r,pivot=a[l+(r-l)/2];
  while(i<=j){while(true){++s.comparisons;if(!(a[i]<pivot))break;++i;}
   while(true){++s.comparisons;if(!(a[j]>pivot))break;--j;}
   if(i<=j){if(i!=j){swap(a[i],a[j]);s.moves+=3;}++i;--j;}
  }
  if(j-l<r-i){if(l<j)quickRange(a,l,j,s);l=i;}
  else{if(i<r)quickRange(a,i,r,s);r=j;}
 }
}
void quickSort(vector<int>& a,Stats& s){if(!a.empty())quickRange(a,0,(int)a.size()-1,s);}
struct Method { const char* name; Sort run; };
const vector<Method> methods={
 {"Insertion",insertion},{"Merge",mergeSort},{"Shell",shell},
 {"Selection",selection},{"Quick",quickSort},{"BinaryInsertion",binaryInsertion},{"Shaker",shaker}
};
vector<int> makeData(int n,int kind,unsigned seed){
 vector<int> a(n);mt19937 gen(seed);uniform_int_distribution<int> dist(-100000,100000);
 for(int& x:a)x=dist(gen);
 if(kind==1)sort(a.begin(),a.end());
 if(kind==2)sort(a.rbegin(),a.rend());
 return a;
}
void selfTest(){
 vector<vector<int>> tests={{},{5},{2,1},{3,1,2,1,3},{-3,0,-3,9,2}};
 for(const auto& method:methods)for(auto sample:tests){
  auto expected=sample;sort(expected.begin(),expected.end());Stats s;method.run(sample,s);
  if(sample!=expected)throw runtime_error(string("Self-test failed: ")+method.name);
 }
}
void experiment(int variant,const vector<int>& sizes,const string& csv){
 selfTest();ofstream out(csv);
 if(!out)throw runtime_error("Cannot open CSV output");
 out<<"variant,method,initial_order,N,time_ms,comparisons,moves,correct\n";
 const char* orders[]={"random","sorted","reverse"};
 cout<<"Variant "<<variant<<" / Вариант "<<variant<<"\n";
 for(int n:sizes)for(int kind=0;kind<3;++kind){
  const auto original=makeData(n,kind,2026u+(unsigned)n);
  auto expected=original;sort(expected.begin(),expected.end());
  for(const auto& method:methods){
   auto data=original;Stats s;
   const auto start=chrono::steady_clock::now();
   method.run(data,s);
   const auto finish=chrono::steady_clock::now();
   bool correct=(data==expected);
   double ms=chrono::duration<double,milli>(finish-start).count();
   out<<variant<<','<<method.name<<','<<orders[kind]<<','<<n<<','
      <<fixed<<setprecision(6)<<ms<<','<<s.comparisons<<','<<s.moves<<','
      <<(correct?"YES":"NO")<<'\n';
   if(!correct)throw runtime_error(string("Incorrect sort: ")+method.name);
   cout<<setw(5)<<n<<" "<<setw(7)<<orders[kind]<<" "<<setw(16)<<method.name
       <<" "<<fixed<<setprecision(4)<<ms<<" ms; comparisons="<<s.comparisons
       <<"; moves="<<s.moves<<" OK\n";
  }
 }
 cout<<"Results saved: "<<csv<<"\n";
}

int main(){try{experiment(8,{1200,2000,7500},"variant8_results.csv");return 0;}catch(const exception& e){cerr<<e.what()<<'\n';return 1;}}
