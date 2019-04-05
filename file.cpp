#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <bits/stdc++.h>
#include <cstdio>
#include <sstream>
#include <regex>

using namespace std;

/*
	delete this comment before submitting
	use as much code as available online so that it is clean and handles error gracefully
	use the facilities provided by c++ like vectors and hash tables and keep the code small and elegent
	don't write un-necessary functions because that will take time and induce errors
	comment the code as much as possible
*/
/////////////////////////////////////////
// char* assemblyCode;									/
int lineNoCounter = 1;									//
char buffer[100];												//
vector< vector<string> > tokCode;
vector< vector<int> > basicBlock;
vector<string> code;
vector<vector<string>> changedUsed;//lineNo, changed, used, used.
/////////////////////////////////////////

void translateInstr(vector<string> code_in){
	// for every block each intruction will be converted one by one using translateInstr function
	for(int i=0;i<code_in.size();i++){
		cout << code_in[i] << '\n';
	}
}

void print2d(vector<vector<string>> vect){
	for (int i = 0; i < vect.size(); i++) {
        for (int j = 0; j < vect[i].size(); j++)
            cout << vect[i][j] << " ";
        cout << endl;
    }
}
void print2dInt(vector<vector<int>> vect){
	for (int i = 0; i < vect.size(); i++) {
        for (int j = 0; j < vect[i].size(); j++)
            cout << vect[i][j] << " ";
        cout << endl;
    }
}
vector<string> split_string(const string& str, const string& delimiter){
    vector<string> strings;
    string::size_type pos = 0;
    string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    strings.push_back(str.substr(prev));
    return strings;
}

vector<string> splitStrings(string str, char dl)
{
    string word = "";

    int num = 0;
    str = str + dl;
    int l = str.size();
    vector<string> substr_list;
    for (int i = 0; i < l; i++) {
        if (str[i] != dl)
            word = word + str[i];

        else {

            if ((int)word.size() != 0)
                substr_list.push_back(word);
            word = "";
        }
    }

    return substr_list;
}
void tokenize(){
		vector<string> temp;
    regex isFun("(@)(.*)");
		regex isLabel("([a-zA-Z]+)(~)(.*)");
		regex isGoto("(goto)(.*)");
		regex isIfGoto("(if)(.*)");
		regex isEq("(=)");
		regex isPlusM("(\\+|-)");
		regex isMul("\\*|/");
		regex isOp("(gt|lt|geq|leq|and|or)");
		regex isRet("(return)");
		regex isCall("(call)");
		regex isPrint("(prnt)");
		regex isScan("(scan)");
		regex isEqCall("(= call)");
		for (int j = 0; j < code.size(); j++) {
			auto strings = split_string(code[j], " ");
			int i = 1;

			if (regex_match(code[j],isFun)) {
				temp.push_back("1");
			}
			else	if (regex_match(code[j],isLabel)) {
				temp.push_back("2");
			}
			else	if (regex_match(code[j],isGoto)) {
				temp.push_back("3");
			}
			else	if (regex_match(code[j],isIfGoto)) {
				temp.push_back("4");
			}
			else	if (regex_search(code[j],isEq) && regex_search(code[j],isCall)) {
				temp.push_back("9");
			}
			else	if (regex_search(code[j],isEq)) {
				temp.push_back("6");
			}
			else	if (regex_search(code[j],isRet)) {
				temp.push_back("7");
			}

			else	if (regex_search(code[j],isCall)) {
				temp.push_back("8");
			}
			else	if (regex_search(code[j],isPrint)) {
				temp.push_back("10");
			}
			else	if (regex_search(code[j],isScan)) {
				temp.push_back("11");
			}
			else	if ( regex_search(code[j],isEq) &&
							 ( regex_search(code[j],isOp) || regex_search(code[j],isMul)
							 || regex_search(code[j],isPlusM)
								)
								) {
				temp.push_back("5");
			}
			// else	if (regex_match(code[j],isAssgn)) {
			// 	temp.push_back("6");
			// }
			for (auto itr = strings.begin(); itr != strings.end(); itr++){
					// cout << i++ << " - \"" << *itr << "\"\n";
					temp.push_back(*itr);
					}
			tokCode.push_back(temp);
			temp.clear();
		}
}

void updateChangedUsed() {
	vector<string> temp;
	regex dot("(\\.)");

	for (int j = 0; j < tokCode.size(); j++) {
		temp.clear();
		if (tokCode[j].size() - 1 == 1) { // Label, Function Label or Return
			continue;
		}
		else if (tokCode[j].size() - 1 == 2) { // Prnt, Scan, Goto, CallWithoutRetrun, Return with variable
			if (tokCode[j][1] == "prnt") {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back("null");   // changed
				temp.push_back(tokCode[j][2]); // used1.
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
			else if (tokCode[j][1] == "scan") {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][2]);   // changed
				temp.push_back("null"); // used1.
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
			else if (regex_search(tokCode[j][2],dot) && tokCode[j][1] == "return") {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back("null");   // changed
				temp.push_back(tokCode[j][2]); // used1.
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
			else if (tokCode[j][1] == "return") {
				continue;
			}
		}
		else if (tokCode[j].size() - 1 == 3) { // Assignment
			if (regex_search(tokCode[j][3], dot)) {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back(tokCode[j][3]); // used1.
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
			else{
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back("null"); // used1
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
		}
		else if (tokCode[j].size() - 1 == 4) { // call with return
			if (tokCode[j][3] == "call" ) {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back("null"); // used1
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
		}
		else if (tokCode[j].size() - 1 == 5) { // operation
			if (regex_search(tokCode[j][3], dot) && regex_search(tokCode[j][5], dot)) {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back(tokCode[j][3]); // used1
				temp.push_back(tokCode[j][5]); // used.
				changedUsed.push_back(temp);
			}
			else if (regex_search(tokCode[j][3], dot)){
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back(tokCode[j][3]); // used1
				temp.push_back("null");   // used2
				changedUsed.push_back(temp);
			}
			else if (regex_search(tokCode[j][5], dot)){
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back("null");   // used1
				temp.push_back(tokCode[j][5]); // used2.
				changedUsed.push_back(temp);
			}
			else {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back(tokCode[j][1]);   // changed
				temp.push_back("null");   // used1
				temp.push_back("null"); // used2.
				changedUsed.push_back(temp);
			}
		}
		else if (tokCode[j].size() == 6) { // ifGoto
			if (regex_search(tokCode[j][3], dot)) {
				sprintf(buffer, "%d", j);
				temp.push_back(buffer); // lineNo inserted.
				temp.push_back("null");   // changed
				temp.push_back(tokCode[j][3]); // used1
				temp.push_back("null"); // used.
				changedUsed.push_back(temp);
			}
		}
	}
}
// tokenizes the code vector into tokCode[][] vector where each
// instrunction starts with a LINENUMBER.
///////////////////////////////////////////////////////////////////

typedef struct functionArg
 {
 	string funName;
 	int numberOfArg;
 	vector<string> arglist;
 }functionArg;


void process(string* line) {
	string str1 ("");
    if(line->compare(str1) == 0){

	}
	else{
		code.push_back(*line);
	}
}



void removeWord(string* s,string* word){
	//this function removes all occurences of "word" in string "s"

	int i,length;
	size_t firstIndex;
	length = (*word).size();

	while(1){
		firstIndex = s->find(*word);
		if(firstIndex!=string::npos){
			s->erase (firstIndex,length);
		}
		else{
			break;
		}
	}
	return;

}

void clean(string& s){
	string temp("call");
	int lastindex;
	lastindex = s.find(temp);
	lastindex +=4;
	s.erase(0,lastindex);
	return;
}


void AlterCode(){
	    std::vector<string> funNameArg;			//vector to store the arguments of a function with its name
	    string tempStr("");
	    string str1("@");
	    string str2("param");
	    string str3(":");
	    string str4(" ");
	    string currentFunction;
	    size_t found,found1;
	    int v=1,count;
		   for(int i=0;i<code.size();i++){
		   	found1 = code[i].find(str1);
		   	if(found1 != string::npos){
		   		v=1;
		   		currentFunction=code[i];
		   		removeWord(&currentFunction,&str1);		//@ is removed
		   		removeWord(&currentFunction,&str3);		// : is removed
		   	}
		  	found = code[i].find(str2);
		   	if (found != string::npos) {
		   		//the word "param" is in code[i]
		   		removeWord(&code[i],&str2);		//removing the word "param"
		   		removeWord(&code[i],&str4);		//removing the extra spaces
		   		code[i]+= " = "+currentFunction+"."+to_string(v);
		   		v++;
	     	}
	     }
	    //till now all the "param" word are replaced correctly


	     str2 = "call";
	     string currentCall;
	     std::vector<string> T;
	     string Fname;
		    for(int i=0;i<code.size();i++){
		   	found1 = code[i].find(str2);
	     	if(found1 != string::npos){
	     		// the word "call is in this line"
	     		// cout<<"i="<<i;
	     		currentCall = code[i];
	     		clean(currentCall);
	     		auto string_s = splitStrings(currentCall, ' ');
	     		for (auto itr = string_s.begin(); itr != string_s.end(); itr++){
						T.push_back(*itr);
				}
				//currentCall is tokenized and in T
				Fname=T[0];
				// cout<<"\n------\n";
				string Gstring;
				Gstring = "";
				for(int k=1;k<T.size();k++){
					Gstring+=Fname+"."+to_string(k)+" = "+T[k];
					code.insert(code.begin()+i,Gstring);
					Gstring = "";
					//cout<<Fname<<"."<<k<<" = "<<T[k]<<endl;
				}

				i+=T.size()-1;
				// cout<<"\n------\n";
				T.clear();
	     	}

	     }
	   	/*
			Note: use my function for tokenization becuase,anuj's function has a problem
			(it is taken from gfg and bookmarked)
	   	*/

	   	//finally extracting the type information of the user variables
	   	unordered_map <string, int> userVariables;
	   	std::vector<string> variables;
	   	str1="int";
	   	str2="char";
	   	str3=" ";
	   	string currentVariable;
	    for(int i=0;i<code.size();i++){
	    	found1 = code[i].find(str1);
	     	if(found1 != string::npos){
	     		// the word "int" is there in the line
	     		currentVariable=code[i];
	     		removeWord(&currentVariable,&str1);		//"int" is removed
	     		removeWord(&currentFunction,&str3);		// spaces are removed
	     		userVariables[currentVariable]=1;
	     		variables.push_back(currentVariable);
	     		code.erase(code.begin()+i);
	     		i--;
	     	}
	     	found = code[i].find(str2);
	     	if(found != string::npos){
	     		// the word "char" is there in the line
	     		currentVariable=code[i];
	     		removeWord(&currentVariable,&str2);		//"char" is removed
	     		removeWord(&currentFunction,&str3);		// spaces are removed
	     		userVariables[currentVariable]=2;
	     		variables.push_back(currentVariable);
	     		code.erase(code.begin()+i);
	     		i--;
	     	}
	    }


}


void findBasicBlock(){
	vector<int> temp;
	int state;
	state=1;
	int index=0;
	int t;
	while(index<tokCode.size()){
		t=stoi(tokCode[index][0]);
		if(t!=3 && t!=4 && t!=1 && t!=2 && t!=7 && t!=8 && t!=9 && state==2){
			//middle of a basic block
			index++;
			continue;
		}
		if(t!=3 && t!=4 && t!=1 && t!=2 && t!=7 && t!=8 && t!=9 && state==1){
			//start of a basic block
			temp.push_back(index);
			index++;
			state=2;
			continue;
		}
		if(t==1 && state==1){
			//start of a basic block
			temp.push_back(index);
			state=2;
			index++;
			continue;
		}
		if(t==2 && state==1){
			//start of a basic block
			temp.push_back(index);
			state=2;
			index++;
			continue;
		}
		if(t==3 && state==1){
			temp.push_back(index);
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			index++;
			continue;
		}
		if(t==4 && state==1){
			temp.push_back(index);
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			index++;
			continue;
		}
		if(t==7 && state==1){
			temp.push_back(index);
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			index++;
			continue;
		}
		if(t==8 && state==1){
			temp.push_back(index);
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			index++;
			continue;
		}
		if(t==9 && state==1){
			temp.push_back(index);
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			index++;
			continue;
		}
		if(t==1 && state==2){
			temp.push_back(index-1);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			continue;
		}
		if(t==2 && state==2){
			temp.push_back(index-1);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			continue;
		}
		if(t==3 && state==2){
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			index++;
			continue;
		}
		if(t==4 && state==2){
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			index++;
			continue;
		}
		if(t==7 && state==2){
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			index++;
			continue;
		}
		if(t==8 && state==2){
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			index++;
			continue;
		}
		if(t==9 && state==2){
			temp.push_back(index);
			basicBlock.push_back(temp);
			temp.clear();
			state=1;
			index++;
			continue;
		}
	}

}

void AlterCode1(){
	int i,j;
	int flag1=0,flag2=0;
	//flag 1 is for both call and = in i-th line
	//flat 2 is for only call in i-th line
	for(i=0;i<tokCode.size();i++){
		flag1=0;
		flag2=0;
		for(j=0;j<tokCode[i].size();j++){
			if(tokCode[i][j].compare("call")==0){

				for(int k=0;k<tokCode[i].size();k++){
					if(tokCode[i][k].compare("=")==0){
						// both call and "="  are in this line
						flag1=1;
					}
				}
				if(flag1!=1){
					flag2=1;
				}
			}
		}
		if(flag1==1){
			for(int l=5;l<tokCode[i].size();){
				tokCode[i].erase(tokCode[i].begin()+l);
			}

			//cout<<"flag1=1 for line"<<i<<endl;
		}

		if(flag2==1){
			for(int l=3;l<tokCode[i].size();){
				tokCode[i].erase(tokCode[i].begin()+l);
			}
			//cout<<"flag2=1 for line"<<i<<endl;

		}

	}
}

int main(int argc, char* argv[]) {
    string line;
    if(argc != 2) {
        cerr << "One argument is required." << endl;
        return 1;
        }
    string filename(argv[1]);
  //  cout << "* trying to open and read: " << filename << endl;
    ifstream f (argv[1]);
    if (!f.is_open())
        perror(("error while opening file " + filename).c_str());

    while(getline(f, line)) {
        process(&line);
        }

    if (f.bad())
        perror(("error while reading file " + filename).c_str());
    f.close();

    // cout<<"finally printing the vector:"<<endl;
    // for(int i=0;i<code.size();i++){
    // 	cout<<code[i]<<endl;
    // }


		AlterCode();

		tokenize();
		// the input is tokenized and it is in tokCode
		findBasicBlock();
		//basic block are found
		AlterCode1();
		//changing the code a little bit

		print2d(tokCode);

		updateChangedUsed();

		print2d(changedUsed);


  return 0;
}
