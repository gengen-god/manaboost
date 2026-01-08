#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* データの個数: 実験をするときはここを変える */
#define n 67108864

/* 整列するデータ: 配列の添字を1から始めるため
要素数を1つ多く宣言する．*/
static int a[n+1];

/* 比較回数を数えるための変数 */
static unsigned long int hikaku = 0;

/* 補助関数: 実行時間を測定するための関数 */
double getcputime(void)
{
/*clock() はプログラム開始からの経過CPU時間を返す
CLOCK_PER_SEC で割ることで秒単位に変換する*/
return (double)clock() / CLOCKS_PER_SEC;
}

/*** マージソート ***/
/* 整列済みの二つの部分配列をマージする． */
void merge(int p, int m)
{
static int b[n+1]; /* マージするときに一時的に使う配列 */
int i, j, k, h;

/* 配列aのa[p]～a[p+m]は，
a[p]～a[p+h-1] はソート済み，a[p+h]～a[p+m-1] もソート済みなので，
これらを順に先頭から見て，小さい方を配列bのb[p]～b[p+m-1]に代入する．*/
h = m / 2; i = p; j = p + h;
for (k = p; k < p + m ; ++k) {
hikaku++; /* 下記の a[i] <= a[j] の分 */
if ((a[i] <= a[j] && i <p + h) || j >= p + m) {
b[k] = a[i++];
} else {
b[k] = a[j++];
}
}

/* 配列bのb[p]～b[p+m-1]には，
a[p]～a[p+m-1]が整列して入っているので，
それを配列aに戻す．*/
for (k = p; k < p+m; ++k) {
a[k] = b[k];
}
}

/* マージソートの本体 */
void msort(int p, int m)
{
int h;

if (m > 1) {
/* 再帰で記述されているので，プログラムは4行しかない */
h = m / 2;
msort(p, h); /* a[p]～a[p+h-1] までをソート: 再帰 */
msort(p + h, m - h ); /* a[p+h]～a[p+m-1] までをソート: 再帰 */
merge(p, m); /* a[p]～a[p+h-1] と a[p+h]～a[p+m-1] をマージする */
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

/* 整列前の全要素を表示．*/
/*printf("Before:\n");
for (i = 1; i <= n; ++i) {
printf("%10d\n", a[i]);
}
printf("\n");

/* データの整列を実行 */
start = getcputime(); /* 開始時間 */
msort(1, n);
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

