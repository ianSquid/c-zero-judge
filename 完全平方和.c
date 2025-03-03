#include <stdio.h>
#include <math.h>

int main(){
    int a ;
    scanf("%d",&a);
    double b[a*2];
    int i;
    int j;
    int ans[a];
    for(i = 0;i < a; i++)
        ans[i] = 0;
    for(i=0;i < a*2; i++)
        scanf("%lf",&b[i]);
    for(i=0;i<a;i++){
        for(j=b[i];j<b[(i*2)+1]+1;j++){
            if(((int)sqrt(j)*(int)sqrt(j)) == j){
                ans[i]= j + ans[i];
            }
        }
    }
    for(i=0;i<a;i++)
        printf("Case %d: %d\n",i+1,ans[i]);
}