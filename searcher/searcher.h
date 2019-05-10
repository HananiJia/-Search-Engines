#pragma once 
//构建索引模块和搜索模块
#include<string>
#include<vector>
#include<unordered_map>
#include"../cppjieba/include/cppjieba/Jieba.hpp"
namespace searcher {
    struct DocInfo {
        uint64_t doc_id;
        std::string title;
        std::string content;
        std::string url;
    };
    struct Weight{
      uint64_t doc_id;
      int weight;
      std::string key;
    };
    typedef std::vector<Weight> InvertedList;
    class Index {
        private:
            std::vector<DocInfo> forward_index_;
            std::unordered_map<std::string,InvertedList> inverted_index_;
            const DocInfo* BuildForward(const std::string& line);
            void BuildInverted(const DocInfo& doc_info);
            cppjieba::Jieba jieba_;
        public:
        bool Build(const std::string& input_path);
        const DocInfo* GetDocInfo(uint64_t doc_id) const;
        const InvertedList* GetInvertedList(const std::string& key) const;
        void CutWord(const std::string& input,std::vector<std::string>* output);
        Index();
    };

    class Searcher {
     private:
         Index* index_;
         std::string GetDesc(const std::string& content,const std::string& key);    
     public:
         Searcher():index_(new Index()){
         }
         ~Searcher(){
          delete index_;
         }
     bool Init(const std::string& input_path);    
     bool Search(const std::string& query,std::string* result);

    };
}
