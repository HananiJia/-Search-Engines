#include"searcher.h"
#if 0
int main(){
    searcher::Index index;
    bool ret=index.Build("../data/tmp/raw_input");
    if(!ret){
      std::cout<<" Build failed"<<std::endl;
      return 1;
    }
    auto* inverted_list=index.GetInvertedList("filesystem");
    if(inverted_list==NULL){
      std::cout<<"GetInvertedList failed"<<std::endl;
      return 1;
    }
    for (auto weight:*inverted_list){
       std::cout<<"id:"<<weight.doc_id<<"weight"<<weight.weigth<<std::endl;
       const auto* doc_info=index.GetDocInfo(weight.doc_id);
       std::cout<<"title"<<doc_info->title<<std::endl;
       std::cout<<"url"<<doc_info->url<<std::endl;       
    }
    return 0;
}
#endif
int main() {
     searcher::Searcher searcher;
     bool ret= searcher.Init("../data/tmp/raw_input");
     if (!ret){
     return 1;
     }
     std::string query ="filesystem";
     std::string result;
     searcher.Search(query,&result);
     std::cout<<"result"<<result;
    return 0;
}
