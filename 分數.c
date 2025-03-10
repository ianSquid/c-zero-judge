#include <stdio.h>
#include<math.h>
int main(){
    int i;
    float a ,b;
    float d[10]={0};
    int c[10]={10,30,36,2,69,79,99,1,48,80};
    for(i=0;i<10;i++){
        if(c[i]<36){
            d[i]=c[i];
        }
        else if(c[i]<65){
            d[i]=sqrt(c[i])*10.2;
        }
        else if(c[i]<101){
            d[i]=sqrt(c[i])*10;
        }
    }
    for(i=0;i<10;i++){
        printf("%f ",d[i]);
    }
}
