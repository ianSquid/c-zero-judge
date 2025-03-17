#include <stdio.h>
#include <math.h>
int maxpool(int a,int b,int c,int d);
int main(){
    int n;
    scanf("%d",&n);
    int m[n][n];
    int ans[n/2][n/2];
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            scanf("%d",&m[i][j]);
        }
    }
    ///////////////////////////////////////////////////////
    for(int i = 0; i < n; i += 2){
        for(int j = 0; j < n; j += 2){
            ans[i/2][j/2] = maxpool(m[i][j],m[i][j+1],m[i+1][j],m[i+1][j+1]);
        }
    }
    printf("\n");
    ///////////////////////////////////////////////////////
    for(int i = 0; i < (n / 2); i++){
        for(int j = 0; j < (n / 2); j++){
            printf("%d ",ans[i][j]);
        }
        printf("\n");
    }
}

int maxpool(int a,int b,int c,int d){
    int t1,t2,max;
    t1 = ((a > b) || (a == b)) ? a : b;
    t2 = ((c > d) || (c == d)) ? c : d;
    max = ((t1 > t2) || (t1 == t2)) ? t1 : t2;
    return max;
}


