#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
namespace n {
#ifdef _WIN32
std::string guiText;
LRESULT CALLBACK NovaWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
 if (msg == WM_CREATE) {
  CreateWindowA("STATIC", guiText.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
   16, 16, 440, 220, hwnd, nullptr, GetModuleHandleA(nullptr), nullptr);
  return 0;
 }
 if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
 return DefWindowProcA(hwnd, msg, wp, lp);
}
#endif
void openWindow(const std::string& title, int width, int height, const std::string& text, int line) {
 if (width < 100 || height < 100 || width > 4000 || height > 4000)
  throw Err(line, "Window dimensions must be between 100 and 4000 pixels.");
#ifdef _WIN32
 guiText = text;
 HINSTANCE inst = GetModuleHandleA(nullptr);
 const char* cls = "NovaScriptWindow";
 WNDCLASSA wc{};
 wc.lpfnWndProc = NovaWindowProc;
 wc.hInstance = inst;
 wc.lpszClassName = cls;
 wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
 wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
 RegisterClassA(&wc);
 HWND hwnd = CreateWindowA(cls, title.c_str(), WS_OVERLAPPEDWINDOW,
  CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, inst, nullptr);
 if (!hwnd) throw Err(line, "Could not create window.");
 ShowWindow(hwnd, SW_SHOW);
 UpdateWindow(hwnd);
 MSG msg;
 while (GetMessageA(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageA(&msg); }
#else
 (void)title; (void)text;
 throw Err(line, "The window() built-in is currently supported on Windows only.");
#endif
}

struct Err:std::runtime_error{int line;Err(int l,const std::string&m):runtime_error(m),line(l){}};
struct V{enum K{Num,Str,Bool,Nil} k=Nil;double d=0;std::string s;bool b=false;
 static V num(double x){V v;v.k=Num;v.d=x;return v;} static V str(std::string x){V v;v.k=Str;v.s=x;return v;} static V boo(bool x){V v;v.k=Bool;v.b=x;return v;}
 std::string show()const{if(k==Num){std::ostringstream o;o<<d;return o.str();}if(k==Str)return s;if(k==Bool)return b?"true":"false";return "nil";}
 bool truth()const{return k==Nil?false:k==Bool?b:true;}};
enum T{END,NUMBER,STRING,ID,LP,RP,LB,RB,SEMI,COMMA,PLUS,MINUS,STAR,SLASH,PERCENT,BANG,NE,EQ,EE,LT,LE,GT,GE,LET,SAY,PRINT,IF,ELSE,WHILE,TRUE,FALSE,NIL,AND,OR};
struct Tok{T t;std::string s;int l;};
struct Lex{std::string s;size_t i=0;int line=1;std::vector<Tok> v;
 char peek(int n=0){return i+n<s.size()?s[i+n]:0;} char take(){char c=peek();if(c){i++;if(c=='\n')line++;}return c;}
 void add(T t,size_t a,int l){v.push_back({t,s.substr(a,i-a),l});}
 std::vector<Tok> run(){while(peek()){if(peek()==' '||peek()=='\r'||peek()=='\t'||peek()=='\n'){take();continue;}
 if(peek()=='#'||(peek()=='/'&&peek(1)=='/')){while(peek()&&peek()!='\n')take();continue;}
 size_t a=i;int l=line;char c=take();
 if(c>='0'&&c<='9'){while((peek()>='0'&&peek()<='9')||peek()=='.')take();add(NUMBER,a,l);continue;}
 if(c=='"'||c=='\''){std::string x;while(peek()&&peek()!=c){char q=take();if(q=='\\'&&peek()){q=take();x+=q=='n'?'\n':q=='t'?'\t':q;}else x+=q;}if(!peek())throw Err(l,"Unterminated string.");take();v.push_back({STRING,x,l});continue;}
 if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'){while((peek()>='a'&&peek()<='z')||(peek()>='A'&&peek()<='Z')||(peek()>='0'&&peek()<='9')||peek()=='_')take();std::string w=s.substr(a,i-a);
 static const std::map<std::string,T> kw={{"let",LET},{"say",SAY},{"print",PRINT},{"if",IF},{"else",ELSE},{"while",WHILE},{"true",TRUE},{"false",FALSE},{"nil",NIL},{"and",AND},{"or",OR}};
 auto z=kw.find(w);v.push_back({z==kw.end()?ID:z->second,w,l});continue;}
 auto two=[&](char x,T yes,T no){if(peek()==x){take();v.push_back({yes,s.substr(a,i-a),l});}else v.push_back({no,s.substr(a,i-a),l});};
 switch(c){case '(':add(LP,a,l);break;case ')':add(RP,a,l);break;case '{':add(LB,a,l);break;case '}':add(RB,a,l);break;case ';':add(SEMI,a,l);break;case ',':add(COMMA,a,l);break;case '+':add(PLUS,a,l);break;case '-':add(MINUS,a,l);break;case '*':add(STAR,a,l);break;case '/':add(SLASH,a,l);break;case '%':add(PERCENT,a,l);break;case '!':two('=',NE,BANG);break;case '=':two('=',EE,EQ);break;case '<':two('=',LE,LT);break;case '>':two('=',GE,GT);break;default:throw Err(l,std::string("Unexpected character '")+c+"'.");}}
 v.push_back({END,"",line});return v;}
};
struct Parser{
 std::vector<Tok> t;size_t p=0;std::vector<std::map<std::string,V>> scopes{{}};
 Parser(std::vector<Tok>x):t(std::move(x)){} Tok cur(){return t[p];}bool ck(T x){return cur().t==x;}bool mt(T x){if(ck(x)){p++;return true;}return false;}Tok need(T x,std::string m){if(ck(x))return t[p++];throw Err(cur().l,m);}void semi(){mt(SEMI);}
 V get(std::string s,int l){for(auto i=scopes.rbegin();i!=scopes.rend();++i){auto f=i->find(s);if(f!=i->end())return f->second;}throw Err(l,"Undefined variable '"+s+"'.");}
 bool set(std::string s,V v){for(auto i=scopes.rbegin();i!=scopes.rend();++i){auto f=i->find(s);if(f!=i->end()){f->second=v;return true;}}return false;}
 V expression(){return logicOr();}
 V logicOr(){V a=logicAnd();while(mt(OR)){V b=logicAnd();a=V::boo(a.truth()||b.truth());}return a;}
 V logicAnd(){V a=equality();while(mt(AND)){V b=equality();a=V::boo(a.truth()&&b.truth());}return a;}
 V equality(){V a=compare();while(ck(EE)||ck(NE)){T op=t[p++].t;V b=compare();bool e=a.k==b.k&&(a.k==V::Num?a.d==b.d:a.k==V::Str?a.s==b.s:a.k==V::Bool?a.b==b.b:true);a=V::boo(op==EE?e:!e);}return a;}
 V compare(){V a=term();while(ck(LT)||ck(LE)||ck(GT)||ck(GE)){T op=t[p++].t;V b=term();nums(a,b);a=V::boo(op==LT?a.d<b.d:op==LE?a.d<=b.d:op==GT?a.d>b.d:a.d>=b.d);}return a;}
 void nums(V a,V b){if(a.k!=V::Num||b.k!=V::Num)throw Err(cur().l,"Operator requires numbers.");}
 V term(){V a=factor();while(ck(PLUS)||ck(MINUS)){T op=t[p++].t;V b=factor();if(op==PLUS&&(a.k==V::Str||b.k==V::Str))a=V::str(a.show()+b.show());else{nums(a,b);a=V::num(op==PLUS?a.d+b.d:a.d-b.d);}}return a;}
 V factor(){V a=unary();while(ck(STAR)||ck(SLASH)||ck(PERCENT)){T op=t[p++].t;V b=unary();nums(a,b);if((op==SLASH||op==PERCENT)&&b.d==0)throw Err(cur().l,"Division by zero.");a=V::num(op==STAR?a.d*b.d:op==SLASH?a.d/b.d:std::fmod(a.d,b.d));}return a;}
 V unary(){if(mt(BANG))return V::boo(!unary().truth());if(mt(MINUS)){V a=unary();if(a.k!=V::Num)throw Err(cur().l,"Unary minus requires a number.");return V::num(-a.d);}return primary();}
 V primary(){if(mt(NUMBER))return V::num(std::stod(t[p-1].s));if(mt(STRING))return V::str(t[p-1].s);if(mt(TRUE))return V::boo(true);if(mt(FALSE))return V::boo(false);if(mt(NIL))return V{};if(mt(ID)){Tok name=t[p-1];if(mt(LP)){if(name.s=="input"){V prompt=expression();need(RP,"Expected ')' after input prompt.");if(prompt.k!=V::Str)throw Err(name.l,"input() prompt must be a string.");std::cout<<prompt.s;std::cout.flush();std::string answer;if(!std::getline(std::cin,answer))answer="";return V::str(answer);}if(name.s=="window"){V title=expression();need(COMMA,"Expected ',' after window title.");V w=expression();need(COMMA,"Expected ',' after window width.");V h=expression();need(COMMA,"Expected ',' after window height.");V body=expression();need(RP,"Expected ')' after window arguments.");if(title.k!=V::Str||body.k!=V::Str||w.k!=V::Num||h.k!=V::Num)throw Err(name.l,"window() expects (string, number, number, string).");openWindow(title.s,static_cast<int>(w.d),static_cast<int>(h.d),body.s,name.l);return V{};}throw Err(name.l,"Unknown built-in '" + name.s + "'.");}return get(name.s,name.l);}if(mt(LP)){V a=expression();need(RP,"Expected ')' after expression.");return a;}throw Err(cur().l,"Expected expression.");}
 size_t matching(size_t start){int d=1;for(size_t i=start;i<t.size();i++){if(t[i].t==LB)d++;if(t[i].t==RB&&--d==0)return i+1;}throw Err(cur().l,"Unclosed block.");}
 void block(){scopes.emplace_back();while(!ck(RB)&&!ck(END))stmt();need(RB,"Expected '}' after block.");scopes.pop_back();}
 void skip(){size_t e=matching(p);p=e;}
 void stmt(){if(mt(SEMI))return;
 if(mt(LET)){Tok x=need(ID,"Expected variable name.");need(EQ,"Expected '=' after variable name.");V a=expression();scopes.back()[x.s]=a;semi();return;}
 if(mt(SAY)||mt(PRINT)){V a=expression();std::cout<<a.show()<<'\n';semi();return;}
 if(mt(IF)){need(LP,"Expected '(' after if.");V c=expression();need(RP,"Expected ')' after condition.");need(LB,"Expected '{' before body.");if(c.truth())block();else skip();if(mt(ELSE)){need(LB,"Expected '{' after else.");if(!c.truth())block();else skip();}return;}
 if(mt(WHILE)){need(LP,"Expected '(' after while.");size_t cond=p;expression();need(RP,"Expected ')' after condition.");need(LB,"Expected '{' before loop body.");size_t body=p,after=matching(body);while(true){p=cond;V c=expression();if(!c.truth())break;p=body;block();}p=after;return;}
 if(mt(LB)){block();return;}if(ck(ID)&&p+1<t.size()&&t[p+1].t==EQ){Tok x=t[p++];p++;V a=expression();if(!set(x.s,a))throw Err(x.l,"Undefined variable '"+x.s+"'.");semi();return;}expression();semi();}
 void run(){while(!ck(END))stmt();}
};
int run(const std::string&s){try{Parser p(Lex{s}.run());p.run();return 0;}catch(const Err&e){std::cerr<<"Nova error (line "<<e.line<<"): "<<e.what()<<'\n';return 1;}catch(const std::exception&e){std::cerr<<"Nova error: "<<e.what()<<'\n';return 1;}}
}
int main(int argc,char**argv){if(argc>1&&std::string(argv[1])=="--version"){std::cout<<"Nova 1.0.0\n";return 0;}if(argc>1&&(std::string(argv[1])=="--help"||std::string(argv[1])=="-h")){std::cout<<"Nova 1.0.0\nUsage: nova [file.nova]\n       nova --help\n       nova --version\n";return 0;}if(argc>2){std::cerr<<"Usage: nova [file.nova]\n";return 2;}if(argc==2){std::ifstream f(argv[1]);if(!f){std::cerr<<"Cannot open file: "<<argv[1]<<'\n';return 2;}std::ostringstream s;s<<f.rdbuf();return n::run(s.str());}std::cout<<"Nova 1.0.0 — interactive mode (Ctrl+D/Ctrl+Z to exit)\n";std::string line;while(std::cout<<"> "&&std::getline(std::cin,line))if(!line.empty())n::run(line);return 0;}
