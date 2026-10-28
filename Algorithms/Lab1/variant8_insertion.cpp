#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
struct Statistics { std::uint64_t comparisons=0, moves=0; };
using Algorithm=void(*)(std::vector<int>&,Statistics&);
void insertionSort(std::vector<int>& a,Statistics& s){
 for(std::size_t i=1;i<a.size();++i){int key=a[i];++s.moves;std::size_t j=i;
  while(j>0){++s.comparisons;if(a[j-1]<=key)break;a[j]=a[j-1];++s.moves;--j;}
  a[j]=key;++s.moves;
 }
}
void shellSort(std::vector<int>& a,Statistics& s){
 for(std::size_t gap=a.size()/2;gap>0;gap/=2)
  for(std::size_t i=gap;i<a.size();++i){int key=a[i];++s.moves;std::size_t j=i;
   while(j>=gap){++s.comparisons;if(a[j-gap]<=key)break;a[j]=a[j-gap];++s.moves;j-=gap;}
   a[j]=key;++s.moves;
  }
}
void selectionSort(std::vector<int>& a,Statistics& s){
 for(std::size_t i=0;i<a.size();++i){std::size_t m=i;
  for(std::size_t j=i+1;j<a.size();++j){++s.comparisons;if(a[j]<a[m])m=j;}
  if(m!=i){std::swap(a[i],a[m]);s.moves+=3;}
 }
}
void binaryInsertionSort(std::vector<int>& a,Statistics& s){
 for(std::size_t i=1;i<a.size();++i){int key=a[i];++s.moves;std::size_t lo=0,hi=i;
  while(lo<hi){std::size_t mid=lo+(hi-lo)/2;++s.comparisons;if(a[mid]<=key)lo=mid+1;else hi=mid;}
  for(std::size_t j=i;j>lo;--j){a[j]=a[j-1];++s.moves;}a[lo]=key;++s.moves;
 }
}
void shakerSort(std::vector<int>& a,Statistics& s){
 if(a.size()<2)return;std::size_t left=0,right=a.size()-1;
 while(left<right){bool swapped=false;
  for(std::size_t i=left;i<right;++i){++s.comparisons;if(a[i]>a[i+1]){std::swap(a[i],a[i+1]);s.moves+=3;swapped=true;}}
  if(!swapped)break;--right;swapped=false;
  for(std::size_t i=right;i>left;--i){++s.comparisons;if(a[i-1]>a[i]){std::swap(a[i-1],a[i]);s.moves+=3;swapped=true;}}
  ++left;if(!swapped)break;
 }
}
void mergeRange(std::vector<int>& a,std::vector<int>& t,std::size_t l,std::size_t r,Statistics& s){
 if(r-l<2)return;std::size_t m=l+(r-l)/2;
 mergeRange(a,t,l,m,s);mergeRange(a,t,m,r,s);
 std::size_t i=l,j=m,k=l;
 while(i<m&&j<r){++s.comparisons;t[k++]=(a[i]<=a[j]?a[i++]:a[j++]);++s.moves;}
 while(i<m){t[k++]=a[i++];++s.moves;}while(j<r){t[k++]=a[j++];++s.moves;}
 for(std::size_t p=l;p<r;++p){a[p]=t[p];++s.moves;}
}
void mergeSort(std::vector<int>& a,Statistics& s){std::vector<int> t(a.size());mergeRange(a,t,0,a.size(),s);}
void quickRange(std::vector<int>& a,int l,int r,Statistics& s){
 while(l<r){int i=l,j=r,pivot=a[l+(r-l)/2];
  while(i<=j){
   while(true){++s.comparisons;if(!(a[i]<pivot))break;++i;}
   while(true){++s.comparisons;if(!(a[j]>pivot))break;--j;}
   if(i<=j){if(i!=j){std::swap(a[i],a[j]);s.moves+=3;}++i;--j;}
  }
  if(j-l<r-i){if(l<j)quickRange(a,l,j,s);l=i;}
  else{if(i<r)quickRange(a,i,r,s);r=j;}
 }
}
void quickSort(std::vector<int>& a,Statistics& s){if(!a.empty())quickRange(a,0,static_cast<int>(a.size())-1,s);}
struct Method {const char* name;Algorithm function;};
const std::vector<Method>& allMethods(){
 static const std::vector<Method> methods={
  {"Insertion",insertionSort},{"Merge",mergeSort},{"Shell",shellSort},
  {"Selection",selectionSort},{"Quick",quickSort},
  {"BinaryInsertion",binaryInsertionSort},{"Shaker",shakerSort}};
 return methods;
}
void verifyAlgorithms(){
 const std::vector<std::vector<int>> cases={{},{7},{2,1},{5,5,5},{4,-1,4,2,0,-1},{1,2,3,4,5},{5,4,3,2,1}};
 for(const auto& method:allMethods())for(const auto& sample:cases){
  auto data=sample,expected=sample;std::sort(expected.begin(),expected.end());Statistics s;
  method.function(data,s);
  if(data!=expected)throw std::runtime_error(std::string("Test failed: ")+method.name);
 }
}
std::vector<int> makeInput(int n,int order,unsigned seed){
 std::vector<int> a(static_cast<std::size_t>(n));std::mt19937 rng(seed);
 std::uniform_int_distribution<int> dist(-100000,100000);
 for(int& value:a)value=dist(rng);
 if(order==1)std::sort(a.begin(),a.end());
 if(order==2)std::sort(a.rbegin(),a.rend());
 return a;
}
void runExperiment(int variant,const std::vector<int>& sizes,const std::string& outputName){
 verifyAlgorithms();std::ofstream csv(outputName);
 if(!csv)throw std::runtime_error("Cannot create results CSV");
 csv<<"variant,method,input_order,N,time_ms,comparisons,moves,correct\n";
 const char* orders[]={"random","sorted","reverse"};
 std::cout<<"Lab 1 / Part 1 / Variant "<<variant<<"\n";
 for(int n:sizes)for(int order=0;order<3;++order){
  const auto source=makeInput(n,order,2026u+static_cast<unsigned>(n));
  auto expected=source;std::sort(expected.begin(),expected.end());
  for(const auto& method:allMethods()){
   auto data=source;Statistics stats;
   const auto start=std::chrono::steady_clock::now();
   method.function(data,stats);
   const auto finish=std::chrono::steady_clock::now();
   const bool correct=(data==expected);
   const double elapsed=std::chrono::duration<double,std::milli>(finish-start).count();
   csv<<variant<<','<<method.name<<','<<orders[order]<<','<<n<<','
      <<std::fixed<<std::setprecision(6)<<elapsed<<','<<stats.comparisons<<','<<stats.moves<<','
      <<(correct?"YES":"NO")<<'\n';
   if(!correct)throw std::runtime_error(std::string("Incorrect result: ")+method.name);
   std::cout<<std::setw(5)<<n<<" "<<std::setw(7)<<orders[order]<<" "
            <<std::setw(16)<<method.name<<" "<<std::fixed<<std::setprecision(4)
            <<elapsed<<" ms OK\n";
  }
 }
 std::cout<<"Saved: "<<outputName<<"\n";
}

int main(){
 try{runExperiment(8,{1200,2000,7500},"variant8_results.csv");return 0;}
 catch(const std::exception& error){std::cerr<<"Error: "<<error.what()<<'\n';return 1;}
}
