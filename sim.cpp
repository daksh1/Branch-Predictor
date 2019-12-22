#include<bits/stdc++.h>
using namespace std;
char Predict(int index,int *predictiontable){
    if(predictiontable[index]>=2){
        return 't';
    }
    return 'n';
}
string chartobin(char c){
    switch(c)
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'a': return "1010";
        case 'B': return "1011";
        case 'b': return "1011";
        case 'C': return "1100";
        case 'c': return "1100";
        case 'D': return "1101";
        case 'd': return "1101";
        case 'E': return "1110";
        case 'e': return "1110";
        case 'F': return "1111";
        case 'f': return "1111";
        default: return "2222";
    }

}
string hextobin(string s){
    //cout<<s<<"\n";
    string by="";
    int len=s.length();
    for(int i=0;i<len;i++){
        by=by+(chartobin(s[i]));
        }
    return by;
}

int indexfromPC(string binadd,int m,int n){
    
    int len=binadd.length()-2;
    string index=binadd.substr(len-m,m);
    if(n==0){
        return stoi(index,0,2);
    }
    index=index.substr(0,n);
    return stoi(index,0,2);
}
int remainingpartofindex(string binadd,int m,int n){
    if(m == n)
        return 0;
    int len=binadd.length()-2;
    string index=binadd.substr(len-m,m);
    index=index.substr(n,m-n);
    // cout<<binadd<<"--"<<index<<"--"<<m<<"--"<<n<<"--\n";
    // return 0;
    return stoi(index,0,2);
}

int topnbitsofPCindex(string binadd,int n){
    string index=binadd.substr(0,n);
    return stoi(index,0,2);
}
void printstats(int *table,int size,int predictions,int mispredictions,int n){
    cout<<"number of predictions:"<<predictions<<endl;
    cout<<"number of mispredictions:"<<mispredictions<<endl;
    cout<<"misprediction rate:"<<fixed<<setprecision(2)<<float(mispredictions)/float(predictions)*100<<"%"<<endl;
    if(n==0)
    cout<<"FINAL BIMODAL CONTENTS"<<endl;
    else if(n==1)
    cout<<"FINAL GSHARE CONTENTS"<<endl;
    else if(n==-1)
    cout<<"FINAL CHOOSER CONTENTS"<<endl;
    for(int i=0;i<size;i++){
        cout<<i<<" "<<table[i]<<endl;
    }
}
void bimodalfunc(int numPCbits,int BTBsize,int BTBasso,char **argv){
    int PredictionTableSize=pow(2,numPCbits);
    int bimodaltable[PredictionTableSize];
    int mispredictions=0;
    for(int i=0;i<PredictionTableSize;i++){
        bimodaltable[i]=2;
    }
    ifstream cfile(argv[5]);
    if(!cfile) {
        cout<<"File not found";
        return;
    }
    char type;
    string hadd;
    int numberofPredictions=0;
    while(cfile>>hadd>>type){
        numberofPredictions++;
        char prediction=Predict(indexfromPC(hextobin(hadd),numPCbits,0),bimodaltable);
        if(type=='t' && !(bimodaltable[indexfromPC(hextobin(hadd),numPCbits,0)]>=3)){
            bimodaltable[indexfromPC(hextobin(hadd),numPCbits,0)]++;
        }
        else if(type=='n' && !(bimodaltable[indexfromPC(hextobin(hadd),numPCbits,0)]<=0)){
                bimodaltable[indexfromPC(hextobin(hadd),numPCbits,0)]--;
        }
        if(prediction!=type){
            mispredictions++;
        }
    }
    cout<<"COMMAND"<<endl<<"./sim bimodal "<<numPCbits<<" "<<0<<" "<<0<<" "<<argv[5]<<endl<<"OUTPUT"<<endl;
    printstats(bimodaltable,PredictionTableSize,numberofPredictions,mispredictions,0);
}
void gsharefunc(int numPCbits,int numBHTbits,int BTBsize,int BTBasso,char **argv){
    int PredictionTableSize=pow(2,numPCbits);
    int gsharetable[PredictionTableSize];
    int mispredictions=0;
    for(int i=0;i<PredictionTableSize;i++){
        gsharetable[i]=2;
    }
    string branchHistoryRegister(numBHTbits,'0');
    ifstream cfile(argv[6]);
    if(!cfile) {
        cout<<"File not found";
        return;
    }
    string hadd;
    char type;
    int numberofPredictions=0;
    while(cfile>>hadd>>type){
        numberofPredictions++;
        int topnbitsofPCindex=indexfromPC(hextobin(hadd),numPCbits,numBHTbits);
        int index=(topnbitsofPCindex ^ stoi(branchHistoryRegister,0,2));
        index=(index<<(numPCbits-numBHTbits))+remainingpartofindex(hextobin(hadd),numPCbits,numBHTbits);
        char prediction=Predict(index,gsharetable);
        if(type=='t'){
            if(!(gsharetable[index]>=3))
                gsharetable[index]++;
            branchHistoryRegister='1'+branchHistoryRegister.substr(0,numBHTbits-1);
        }
        else if(type=='n'){
            if(!(gsharetable[index]<=0))
                gsharetable[index]--;
            branchHistoryRegister='0'+branchHistoryRegister.substr(0,numBHTbits-1);
        }
        if(prediction!=type){
            mispredictions++;
        }
    }
    cout<<"COMMAND"<<endl<<"./sim gshare "<<numPCbits<<" "<<numBHTbits<<" "<<0<<" "<<0<<" "<<argv[6]<<endl<<"OUTPUT"<<endl;
    printstats(gsharetable,PredictionTableSize,numberofPredictions,mispredictions,1);

}

void hybridfunc(int chooserBits,int numPCbits,int numBHTbits,int bitsToRepresentBimodalIndex,int BTBsize,int BTBasso,char **argv){
    int chooserTableSize=pow(2,chooserBits);
    int chooserTable[chooserTableSize];
    int bimodalTableSize=pow(2,bitsToRepresentBimodalIndex);
    int gshareTableSize=pow(2,numPCbits);
    int bimodalTable[bimodalTableSize];
    int gshareTable[gshareTableSize];
    for(int i=0;i<chooserTableSize;i++){
        chooserTable[i]=1;
    }
    for(int i=0;i<bimodalTableSize;i++){
            bimodalTable[i]=2;
        }
    for(int i=0;i<gshareTableSize;i++){
        gshareTable[i]=2;
    }
    ifstream cfile(argv[8]);
    //ifstream cfile("/home/daksh/Downloads/563project2/traces");
    char type;
    string hadd;
    int numOfPredictions=0;
    int numOfMispredictions=0;
    string branchHistoryRegister(numBHTbits,'0');
    if(!cfile){
        cout<<"File not found";
        return;
    }
    while(cfile>>hadd>>type){
        numOfPredictions++;
        int index_chooserTable=indexfromPC(hextobin(hadd),chooserBits,0);
        int topnbitsofPCindex=indexfromPC(hextobin(hadd),numPCbits,numBHTbits);
        int index_gshareTable=(topnbitsofPCindex ^ stoi(branchHistoryRegister,0,2));
        index_gshareTable=(index_gshareTable<<(numPCbits-numBHTbits))+remainingpartofindex(hextobin(hadd),numPCbits,numBHTbits);
        char prediction_gshare=Predict(index_gshareTable,gshareTable);
        char prediction_bimodal=Predict(indexfromPC(hextobin(hadd),bitsToRepresentBimodalIndex,0),bimodalTable);
        if(chooserTable[index_chooserTable]>=2){
            if(type=='t'){
                if(!(gshareTable[index_gshareTable]>=3))
                    gshareTable[index_gshareTable]++;
                branchHistoryRegister='1'+branchHistoryRegister.substr(0,numBHTbits-1);
            }
            else if(type=='n'){
                if(!(gshareTable[index_gshareTable]<=0))
                    gshareTable[index_gshareTable]--;
                branchHistoryRegister='0'+branchHistoryRegister.substr(0,numBHTbits-1);
            }
            if(prediction_gshare!=type){
                numOfMispredictions++;
            }
        }
        else{
            if(type=='t' && !(bimodalTable[indexfromPC(hextobin(hadd),bitsToRepresentBimodalIndex,0)]>=3)){
                bimodalTable[indexfromPC(hextobin(hadd),bitsToRepresentBimodalIndex,0)]++;
            }
            else if(type=='n' && !(bimodalTable[indexfromPC(hextobin(hadd),bitsToRepresentBimodalIndex,0)]<=0)){
                    bimodalTable[indexfromPC(hextobin(hadd),bitsToRepresentBimodalIndex,0)]--;
            }
            if(prediction_bimodal!=type){
                numOfMispredictions++;
            }
            if(type=='t'){
                branchHistoryRegister='1'+branchHistoryRegister.substr(0,numBHTbits-1);
            }
            else if(type=='n'){
                branchHistoryRegister='0'+branchHistoryRegister.substr(0,numBHTbits-1);
            }

        }
        if(prediction_gshare !=type && prediction_bimodal==type){
            if(!(chooserTable[index_chooserTable]<=0))
                   chooserTable[index_chooserTable]--;
        }
        else if(prediction_gshare==type && prediction_bimodal!=type){
                if(!(chooserTable[index_chooserTable]>=3))
                    chooserTable[index_chooserTable]++;
        }
    }
    cout<<"COMMAND"<<endl<<"./sim hybrid "<<chooserBits<<" "<<numPCbits<<" "<<numBHTbits<<" "<<bitsToRepresentBimodalIndex<<" "<<0<<" "<<0<<" "<<argv[8]<<endl<<"OUTPUT"<<endl;
    printstats(chooserTable,chooserTableSize,numOfPredictions,numOfMispredictions,-1);
    cout<<"FINAL GSHARE CONTENTS"<<endl;
    for(int i=0;i<gshareTableSize;i++){
        cout<<i<<" "<<gshareTable[i]<<endl;
    }
    cout<<"FINAL BIMODAL CONTENTS"<<endl;
    for(int i=0;i<bimodalTableSize;i++){
        cout<<i<<" "<<bimodalTable[i]<<endl;
    }

}
int stringbinaryindextointeger(string indexes){
    if(indexes.length() == 0)
        return 0;
    return stoi(indexes,0,2);
}

// Cache 1
    struct node{
        string tag;
        int ct;
        int db;
    };
    struct cacheset{
        node* set = NULL;
        public:
        cacheset() {
        }
        cacheset(int numberOfNodes) {
            set = new node[numberOfNodes];
            for(int j=0;j<numberOfNodes;j++){
                set[j].ct=j;
                set[j].db=0;
                set[j].tag="0";
            }

        }
    };
    class cache{
        int l1asso;
        int capacity;
        int blocksize;
        int l1reads=0;
        int l1readsmiss=0;
        int l1writes=0;
        int l1writesmiss=0;
        int branchMissBTB=0;
        int PredictionsFromBranchPredictors=0;
        int mispredictionsFromBranchPredictors=0;
        string branchHistoryRegister;
        cacheset carray[10000];
        int numberofsets;
        public:
        cache(int blocksize,int capacity,int l1asso, int l2capacity, int p, int n, string branchHistoryRegister){
            this->blocksize=blocksize;
            this->capacity=capacity;
            this->l1asso=l1asso;
            this->branchHistoryRegister=branchHistoryRegister;
            numberofsets=capacity/(blocksize*l1asso);
            for(int i=0;i<numberofsets;i++){
                carray[i] = cacheset(l1asso);
            }
        }

        public:
        void printl1cacheset(int i) {
            cout<<"set  "<<dec<<i<<" :"<<" ";
                sort(&(this->carray[i].set[0]), &(this->carray[i].set[0+l1asso]), [](node const & a, node const & b) -> bool{ return a.ct < b.ct; } );
                for(int j=0;j<l1asso;j++){
                    cout<<hex<<stringbinaryindextointeger(this->carray[i].set[j].tag)<<" ";
                    if(this->carray[i].set[j].db==1){
                        cout<<'D'<<" || ";
                    }
                    else{
                    cout<<'N'<<" || ";
                    }
                }
                cout<<dec<<"\n";
        }
        void printcache(int branches){
            cout<<"OUTPUT"<<endl;
            cout<<"size of BTB: "<<this->capacity<<"\n";
            cout<<"number of branches: "<<branches<<"\n";
            cout<<"number of predictions from branch predictor: "<<this->PredictionsFromBranchPredictors<<"\n";
            cout<<"number of mispredictions from branch predictor: "<<this->mispredictionsFromBranchPredictors<<"\n";
            cout<<"number of branches miss in BTB and taken: "<<this->branchMissBTB<<"\n";
            cout<<"total mispredictions: "<<(this->mispredictionsFromBranchPredictors+this->branchMissBTB)<<"\n";
            cout<<"misprediction rate:"<<fixed<<setprecision(2)<<(float(this->mispredictionsFromBranchPredictors+this->branchMissBTB)/float(branches))*100<<"%"<<"\n";
            cout<<"FINAL BTB CONTENTS"<<endl;
            for(int i=0;i<numberofsets;i++){
                cout<<"set  "<<i<<"   "<<": ";
                sort(&(this->carray[i].set[0]), &(this->carray[i].set[0+l1asso]), [](node const & a, node const & b) -> bool{ return a.ct < b.ct; } );
                for(int j=0;j<l1asso;j++){
                    if(stringbinaryindextointeger(this->carray[i].set[j].tag)!=0)
                        cout<<hex<<stringbinaryindextointeger(this->carray[i].set[j].tag)<<" ";
                    
                }
                cout<<dec<<"\n";
            }
        }
        int findleastused(int index){
            int leastusedindex=-1;
            int lrumin = -1;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].ct> lrumin){
                    leastusedindex=i;
                    lrumin = this->carray[index].set[i].ct;
                }
            }
            return leastusedindex;
        }
        bool checkifempty(int index){
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].tag==""){
                    return true;
                }
            }
            return false;
        }

        void incrementlru(int index, int leastusedindex) {
            // cout<<"L1 update LRU\n";
            // for(int i=0;i<l1asso;i++)
            //     cout<<this->carray[index].set[i].ct<<" ";
            // cout<<endl;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].ct<(this->carray[index].set[leastusedindex].ct)){
                    this->carray[index].set[i].ct++;
                }
            }
            this->carray[index].set[leastusedindex].ct=0;
            // for(int i=0;i<l1asso;i++)
            //     cout<<this->carray[index].set[i].ct<<" ";
            // cout<<endl;
        }

        void readfromaddress(string binadd,char TakenorNot,int numPCbits,int *predictiontable,int numBHTbits){
            int hitindex;
            int index;
            string indexstring;
            string tag = decodetag(blocksize, capacity, l1asso, binadd);
            if(numberofsets!=1){
            indexstring = decodeindex(blocksize, capacity, l1asso, binadd);}
            if(numberofsets!=1){
            index = stringbinaryindextointeger(indexstring);}
            else{ index=0;}
            int leastusedindex= findleastused(index);
            bool hitFlag = false;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].tag == tag){
                    hitFlag = true;
                    hitindex=i;
                }
            }
            if(!hitFlag){
                //  cout<<"L1 miss\n";
                // Miss
                l1readsmiss++;
                if(TakenorNot=='t')
                    branchMissBTB++;
                if(!checkifempty(index) && this->carray[index].set[leastusedindex].db==1) {
                    this->carray[index].set[leastusedindex].db=0;
                }
                this->carray[index].set[leastusedindex].tag=tag;
                incrementlru(index, leastusedindex);
            }
            else{
                //  cout<<"L1 hit\n";
                PredictionsFromBranchPredictors++;
                incrementlru(index, hitindex);
                char prediction;
                if(numBHTbits==0){
                    prediction=Predict(indexfromPC(binadd,numPCbits,0),predictiontable);
                    if(TakenorNot=='t' && !(predictiontable[indexfromPC(binadd,numPCbits,0)]>=3)){
                        predictiontable[indexfromPC(binadd,numPCbits,0)]++;
                    }
                    else if(TakenorNot=='n' && !(predictiontable[indexfromPC(binadd,numPCbits,0)]<=0)){
                        predictiontable[indexfromPC(binadd,numPCbits,0)]--;
                    }   
                }
                else{
                    int topnbitsofPCindex=indexfromPC(binadd,numPCbits,numBHTbits);
                    int index_gshare=(topnbitsofPCindex ^ stoi(branchHistoryRegister,0,2));
                    index_gshare=(index_gshare<<(numPCbits-numBHTbits))+remainingpartofindex(binadd,numPCbits,numBHTbits);
                    prediction=Predict(index_gshare,predictiontable);
                    if(TakenorNot=='t'){
                        if(!(predictiontable[index_gshare]>=3))
                            predictiontable[index_gshare]++;
                        branchHistoryRegister='1'+branchHistoryRegister.substr(0,numBHTbits-1);
                    }
                    else if(TakenorNot=='n'){
                        if(!(predictiontable[index_gshare]<=0))
                            predictiontable[index_gshare]--;
                        branchHistoryRegister='0'+branchHistoryRegister.substr(0,numBHTbits-1);
                    }
                }
                if(prediction!=TakenorNot){
                    mispredictionsFromBranchPredictors++;
                }
            }
        }

        string decodetag(int blockSize,int l1s,int l1a,string binadd){
            int i=0;
            int bn=l1s/blockSize;
            int sn=bn/l1a;
            int ob=log2(blockSize);
            int ib=log2(l1s/(blockSize*l1a));
            int tagb=24-ob-ib;
            char tag[tagb];
            char index[ib];
            char offset[ob];
            for(i=0;i<tagb;i++){
                tag[i]=binadd[i];
            }
            tag[tagb]='\0';
            //cout<<tag<<endl;
            return tag;
        }
        string decodeindex(int blockSize,int l1s,int l1a,string binadd){
            int i=0;
            int bn=l1s/blockSize;
            int sn=bn/l1a;
            int ob=log2(blockSize);
            int ib=log2(l1s/(blockSize*l1a));
            int tagb=24-ob-ib;
            char tag[tagb];
            char index[ib];
            char offset[ob];
            for(i=0;i<tagb;i++){
                tag[i]=binadd[i];
            }
            tag[tagb]='\0';
            for(int j=0;j<ib;j++){
                index[j]=binadd[i];
                i++;
            }
            index[ib]='\0';
            for(int k=0;k<ob;k++){
                offset[k]=binadd[i];
                i++;
            }
            offset[ob]='\0';
            return index;
        }
    };


int main(int argc,char **argv){
    if(argc==6){
        int numPCbits=stoi(argv[2]);
        int BTBsize=stoi(argv[3]);
        int BTBasso=stoi(argv[4]);
        if(BTBsize==0 && BTBasso==0)
            bimodalfunc(numPCbits,BTBsize,BTBasso,argv);
        else{
            cache c = cache(4, BTBsize, BTBasso,0,0,0,"null");
            int PredictionTableSize=pow(2,numPCbits);
            int bimodaltable[PredictionTableSize];
            for(int i=0;i<PredictionTableSize;i++){
                bimodaltable[i]=2;
            }
            ifstream cfile(argv[5]);
            string hadd;
            char TakenOrNot;
            int numOfBranches=0;
            while(cfile>>hadd>>TakenOrNot){
                numOfBranches++;
                c.readfromaddress(hextobin(hadd),TakenOrNot,numPCbits,bimodaltable,0);
            }
            cout<<"COMMAND"<<endl<<" "<<"./sim "<<argv[1]<<" "<<numPCbits<<" "<<BTBsize<<" "<<BTBasso<<" "<<argv[5]<<endl;
            c.printcache(numOfBranches);
            cout<<endl<<"FINAL BIMODAL CONTENTS"<<endl;
            for(int i=0;i<PredictionTableSize;i++){
                cout<<i<<" "<<bimodaltable[i]<<endl; 
            }

        }
    }
    else if(argc==7){
        int numPCbits=stoi(argv[2]);
        int numBHTbits=stoi(argv[3]);
        int BTBsize=stoi(argv[4]);
        int BTBasso=stoi(argv[5]);
        if(BTBsize==0 && BTBasso==0)
            gsharefunc(numPCbits,numBHTbits,BTBsize,BTBasso,argv);
        else{
            string branchHistoryRegister(numBHTbits,'0');
            cache c = cache(4, BTBsize, BTBasso,0,0,0,branchHistoryRegister);
            int PredictionTableSize=pow(2,numPCbits);
            int gsharetable[PredictionTableSize];
            for(int i=0;i<PredictionTableSize;i++){
                gsharetable[i]=2;
            }
            ifstream cfile(argv[6]);
            string hadd;
            char TakenOrNot;
            int numOfBranches=0;
            while(cfile>>hadd>>TakenOrNot){
                numOfBranches++;
                c.readfromaddress(hextobin(hadd),TakenOrNot,numPCbits,gsharetable,numBHTbits);
            }
            cout<<"COMMAND"<<endl<<" "<<"./sim "<<argv[1]<<" "<<numPCbits<<" "<<numBHTbits<<" "<<BTBsize<<" "<<BTBasso<<" "<<argv[6]<<endl;
            c.printcache(numOfBranches);
            cout<<endl<<"FINAL GSHARE CONTENTS"<<endl;
            for(int i=0;i<PredictionTableSize;i++){
                cout<<i<<" "<<gsharetable[i]<<endl;
            }
        }
    }
    else{
        int chooserBits=stoi(argv[2]);
        int numPCbits=stoi(argv[3]);
        int numBHTbits=stoi(argv[4]);
        int bitsToRepresentBimodalIndex=stoi(argv[5]);
        int BTBsize=stoi(argv[6]);
        int BTBasso=stoi(argv[7]);
        hybridfunc(chooserBits,numPCbits,numBHTbits,bitsToRepresentBimodalIndex,BTBsize,BTBasso,argv);
    }
    return 0;
}