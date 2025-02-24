#include"searcher.h"
#include<iostream>
#include<fstream>
#include<algorithm>
#include<jsoncpp/json/json.h>
#include"../commom/util.hpp"
namespace searcher {
    const char* const DICT_PATH = "../cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "../cppjieba/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "../cppjieba/dict/user.dict.utf8";
    const char* const IDF_PATH = "../cppjieba/dict/idf.utf8";
    const char* const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";

    Index::Index():jieba_(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH){

    }
   const DocInfo* Index::GetDocInfo(uint64_t  doc_id) const {
     if(doc_id >=forward_index_.size()) {
       return NULL;
     }
     return &forward_index_[doc_id];
   }
   const InvertedList* Index::GetInvertedList(const std::string& key) const {
           auto pos=inverted_index_.find(key);
           if(pos==inverted_index_.end()){
             return NULL;
           }
           return &pos->second;
   }
   bool Index::Build(const std::string& input_path){
       std::cout<<"Index Build Start"<<std::endl;
       std::ifstream file(input_path.c_str());
       if(!file.is_open()){
           std::cout<<"Input_path open failed! input_path:"<<input_path<<std::endl;
           return false;
       }
       std::string line;
     while(std::getline(file,line)){
      const DocInfo *doc_info=BuildForward(line);
       BuildInverted(*doc_info);
       
     }  
       std::cout<<"Index Build Finish!"<<std::endl; 
       file.close();
       return true;
   }
   const DocInfo* Index::BuildForward(const std::string& line){
       std::vector<std::string> tokens;
       StringUtil::Split(line,&tokens,"\3");
       if(tokens.size()!=3){
        std::cout<<"tokens not ok"<<std::endl;
        return NULL;
       }
       DocInfo doc_info;
       doc_info.doc_id=forward_index_.size();
       doc_info.title=tokens[0];
       doc_info.url=tokens[1];
       doc_info.content=tokens[2];
       forward_index_.push_back(doc_info);
       return &forward_index_.back();
   }
   void Index::BuildInverted(const DocInfo& doc_info){
   std::vector<std::string> title_tokens;
   CutWord(doc_info.title,&title_tokens);
   std::vector<std::string> content_tokens;
   CutWord(doc_info.content,&content_tokens); 
   struct WordCnt{
     int title_cnt;
     int content_cnt;
   };
   std::unordered_map<std::string,WordCnt> word_cnt;
   for(std::string word:title_tokens){
       boost::to_lower(word);
       ++word_cnt[word].title_cnt;
   }
   for(std::string word:content_tokens){
         boost::to_lower(word);
        ++word_cnt[word].content_cnt;
   }
   for(const auto& word_pair:word_cnt){
       Weight weight;
       weight.doc_id=doc_info.doc_id;
       weight.weight=10*word_pair.second.title_cnt+word_pair.second.content_cnt;
       weight.key=word_pair.first;
      InvertedList& inverted_list= inverted_index_[word_pair.first];
      inverted_list.push_back(weight);
   }
   return ;
   }
  void Index::CutWord(const std::string& input,std::vector<std::string>* output){
     jieba_.CutForSearch(input,*output);
  }
  
  bool Searcher::Init(const std::string& input_path){
    return index_->Build(input_path);
  }
  bool Searcher::Search(const std::string& query,std::string* json_result){
     std::vector<std::string> tokens;
     index_->CutWord(query,&tokens);
     std::vector<Weight> all_token_result;
     for (std::string word:tokens) {
        boost::to_lower(word);
        auto* inverted_list= index_->GetInvertedList(word);
       if(inverted_list==NULL){
          continue;
       }
       all_token_result.insert(all_token_result.end(),inverted_list->begin(),inverted_list->end());
     }
     std::sort(all_token_result.begin(),all_token_result.end(),[](const Weight& w1,const Weight& w2){
             return w1.weight>w2.weight;
             });
     Json::Value results;
     for (const auto& weight:all_token_result){
       const auto* doc_info=index_->GetDocInfo(weight.doc_id);
       if(doc_info==NULL){
        continue;
       }
       Json::Value result;
       result["title"]=doc_info->title;
       result["url"]=doc_info->url;
       result["desc"]=GetDesc(doc_info->content,weight.key);
       results.append(result);
     }
     Json::FastWriter writer;
     *json_result=writer.write(results);
      return true;
  }
  std::string Searcher::GetDesc(const std::string& content,const std::string& key) {
  size_t pos=content.find(key);
  if(pos==std::string::npos){
    if(content.size()<100){
      return content;
    }else{
    return content.substr(0,100)+"...";
    }
  }
  size_t beg=pos<50 ?0:pos-50;
  if(beg+100>=content.size())
  {
    return content.substr(beg);
  }
  return content.substr(beg,100)+"...";
  }
}

