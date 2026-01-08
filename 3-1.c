#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* データの個数: 実験をするときはここを変える */
#define n 16384

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

/*** クイックソート ***/
void partition(int p, int q, int *jp, int *ip)
{
int i, j, s;
int k, temp_pivot;
k = p + (rand() % (q - p + 1));

temp_pivot = a[k];
a[k] = a[p];
a[p] = temp_pivot;

i = p; j = q; s = a[p];
while (i <= j) {
while (1) {
++hikaku; /* 下の比較をカウント */
if (a[i] < s) { ++i; }
else { break; }
}
while (1) {
++hikaku; /* 下の比較をカウント */
if (a[j] > s) { --j; }
else { break; }
}
if (i <= j) {
/* a[i] と a[j] を交換 */
int temp = a[i];
a[i] = a[j];
a[j] = temp;
++i;
--j;
}
}
*jp = j;
*ip = i;
}

/* クイックソート本体: a[p]～a[q]を整列する */
void quicksort(int p, int q)
{
int i, j;

if (p < q) {
partition(p, q, &j, &i);
quicksort(p, j);
quicksort(i, q);
}
}

int main(void)
{
double start; /* 整列開始時間を記録 */
double end; /* 整列終了時間を記録 */
int i;

srand(time(NULL));
a[1] = rand() % 1000;
for (i = 2; i <= n; ++i) {
a[i] = a[i - 1] + (rand() % 100 + 1);
}

/* 整列前の全要素を表示．*/
/*printf("Before:\n");
for (i = 1; i <= n; ++i) {
printf("%10d\n", a[i]);
}
printf("\n");

/* データの整列 */
start = getcputime(); /* 開始時間 */
quicksort(1, n);
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


