#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* データの個数: この値を変化させて実験をする */
#define n 131072

/* 整列するデータ: 配列の添字を1から始めるため
要素数を1つ多く宣言する．*/
static int a[n+1];

/* 要素比較回数を数えるための変数 */
static unsigned long int hikaku = 0;

/* 補助関数: 実行時間を測定するための関数 */
double getcputime(void)
{
/*clock() はプログラム開始からの経過CPU時間を返す
CLOCK_PER_SEC で割ることで秒単位に変換する*/
return (double)clock() / CLOCKS_PER_SEC;
}

/*** 選択法 ****/
void sentaku(void)
{
int i, j, min;

for (j = 1; j <= n-1; ++j) {
min = j;
for (i = j+1; i <= n; ++i) {
hikaku++; /* 下記の比較の分をカウント */
if (a[min] > a[i]) {
min = i ;
}
}
/* a[j] と a[min] を交換 */
int temp = a[j];
a[j] = a[min];
a[min] = temp;
}
}

int main(void)
{
double start; /* 整列開始時間を記録 */
double end; /* 整列終了時間を記録 */
int i;

/* データの作成 */
srand(time(NULL));
for (i = 1; i <= n; ++i) {
a[i] = rand(); /* 乱数 */
}

/*/* 整列前の全要素を表示． */
/*printf("Before:\n");
for (i = 1; i <= n; ++i) {
printf("%10d\n", a[i]);
}
printf("\n"); */

/* データの整列を実行 */
start = getcputime(); /* 開始時間 */
sentaku();
end = getcputime(); /* 終了時間 */

/* 整列後の全要素を表示．*/
/*printf("After:\n");
for (i = 1; i <= n; ++i) {
printf("%10d\n", a[i]);
}
printf("\n");

/* 測定結果の表示 */
printf("hikaku: %lu [kai]\n", hikaku);
printf("jikan: %f [s]\n", end - start);

return 0;
}



