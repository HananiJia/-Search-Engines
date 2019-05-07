// 数据处理模块
// 把Boost 文档中涉及到的html 进行处理
// 1. 去标签
// 2. 把文件进行合并
//    把文档中涉及到的N个HTML的内容合并并且声称一个行文本文件
//    生成的结果是一个大文件，里边包含很多行，每一行对应Boost中的一个html，这么做的目的是为了让后边的索模块处理起来更方便
// 3.对文档的结构进行分析，提取出文档的标题，正文，目标url

#include<string>
#include<vector>
#include<fstream>
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>
#include<../commom/util.hpp>
using namespace std;

const std::string g_input_path ="../data/input/";
const std::string g_output_path ="../data/tmp/raw_input";

struct DocInfo {
   std::string title;
   std::string content;
   std::string url;
};

bool EnumFile(const string& input_path,vector<string>* file_list){
    namespace fs=boost::filesystem;
    fs::path root_path(input_path);
    if (!fs::exists(root_path)) {
      cout<<"input_path not exist! input_path="<<input_path<<endl;
      return false;
    }
    //boost 递归遍历目录，借助一个特殊的迭代器即可
    //下面是构造一个未初始化的迭代器作为遍历结束标记
    fs::recursive_directory_iterator end_iter;
    for(fs::recursive_directory_iterator iter(root_path);iter!=end_iter;++iter) {
      //1.剔除目录
      if (!fs::is_regular_file(*iter))
	      continue;
      if(iter->path().extension()!=".html") 
	      continue;
      //2. 根据扩展名。只保留html
      file_list->push_back(iter->path().string());
    }
    return true;
}
//从 html 中的title提取标题
bool ParseTitle(const string& html,string* title){
    size_t beg=html.find("<title>");
    if(beg==string::npos){
	    cout<<"title not found!"<<endl;
	    return false;
    }
    size_t end=html.find("</title>");
    if(end==string::npos){
	    cout<<"/title not found!"<<endl;
	    return false;
    }
    beg+=string("<title>").size();
    if(beg>=end)
    {
	    cout<<"beg end error"<<endl;
	    return false;
    }
    *title=html.substr(beg,end-beg);
    return true;
}
bool ParseContent(const string& html,string* content){
   bool is_content =true;
   for( auto c :html){
	   if(is_content){
		   if(c=='<'){
			   is_content=false;
		   }else{
			   if(c=='\n'){
				   c=' ';
			   }
			   content->push_back(c);
		   }
	   }else {
		   if(c=='>')
		   {
			   is_content=true;
		   }
	   }
   }
   return true;
}
//https://www.boost.org/doc/libs/1_53_0/doc/
bool ParseUrl(const string& html,string* url){
   string prefix="https://www.boost.org/doc/libs/1_53_0/doc/";
   string tail=file_path.substr(g_input_path.size());
   *url=prefix+tail;
   return true;
}
bool ParseFile(const string& file_path,DocInfo* doc_info) {
   //1.打开文件，读取文件内容
   string html;
   bool ret=FileUtil::Read(file_path,&html);
   if(!ret) {
    cout<<"Read file failed file_path="<<file_path<<endl;
    return false;
   }
   //2.解析标题
   ret=ParseTitle(html,&doc_info->title);
   if(!ret){
     cout<<"ParseTitle faile"<<endl;
     return false;
   }
   //3.解析正文 并且去除html标签
   ret=ParseContent(html,&doc_info->content);
   if(!ret){
    cout<<"ParseContent failed! file_path="<<file_path<<endl;
   }
   //4.解析出url
   ret=ParseUrl(file_path,&doc_info->url);
   if(!ret){
    cout<<"ParseUrl failed file_path="<<file_path<<endl;
    return false;
   }
   return true;
}
int main(){
    //1.枚举出输入路径中所有html的路径
    std::vector<std::string> file_list;
    bool ret=EnumFile(g_input_path,&file_list);
    if(!ret){o
        cout<<"EnumFile failed!"<<endl;
        return 1;
    }
    //验证 Enumfile是不是正确的 
   for (const auto& file_path:file_list){
	   cout<<file_path<<endl;
   }
    ofstream output_file(g_output_path.c_str());
    if (!output_file.is_open()){
        cout<<"open output_file failed! g_output_path="<<g_output_path<<endl;
    //2.依次处理每个枚举出的路径，对该文档进行分析，分析出文件的标题 正文和URL并且进行去标签处理
    for (const auto& file_path:file_list) {
            DocInfo info;
            //输入的是当前要解析的文件路径
            //输出的是解析 之后得到的DocInfo结构
            ret=ParseFile(file_path,&info);
            if(!ret){
                cout<<"ParseFile failed file_path="<<file_path<<endl;
                continue;
    }
    //3.把分析结果按照一行的形式写入到输出文件
     WriteOutput(info,output_file);
    }
    output_file.close();
    return 0;
}
