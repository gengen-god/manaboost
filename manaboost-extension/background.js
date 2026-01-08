const blockedSites = ["youtube.com", "twitter.com", "x.com", "instagram.com"];
const UNLOCK_KEY = "unlockedUntil";

// ブロック対象かどうか
function isBlocked(url) {
  return blockedSites.some(site => url.includes(site));
}

// 解除期限を取得
async function getUnlockedUntil() {
  const data = await chrome.storage.local.get([UNLOCK_KEY]);
  return data[UNLOCK_KEY] || 0;
}

// minutes分だけ解除
async function setUnlockedMinutes(minutes) {
  const until = Date.now() + minutes * 60 * 1000;
  await chrome.storage.local.set({ [UNLOCK_KEY]: until });
}

// URL更新を監視してブロック/解除を制御
chrome.tabs.onUpdated.addListener(async (tabId, changeInfo, tab) => {
  if (!tab.url) return;

  // 1) Flaskの解除URLを踏んだら、解除期限を保存する
  if (tab.url.startsWith("http://127.0.0.1:5000/unlock")) {
    const u = new URL(tab.url);
    const minutes = parseInt(u.searchParams.get("minutes") || "30", 10);

    // 変な値対策（最低1分、最大180分）
    const safeMinutes = Math.min(Math.max(minutes, 1), 180);

    await setUnlockedMinutes(safeMinutes);

    // 解除したことが分かるように休憩ページへ飛ばす（無ければ / でもOK）
    chrome.tabs.update(tabId, { url: "http://127.0.0.1:5000/break" });
    return;
  }

  // 2) ブロック対象サイトに行こうとしたら、解除期限をチェック
  if (isBlocked(tab.url)) {
    const unlockedUntil = await getUnlockedUntil();

    // 解除期限内なら許可
    if (Date.now() < unlockedUntil) return;

    // 期限切れなら学習画面へ強制送還
    chrome.tabs.update(tabId, { url: "http://127.0.0.1:5000/study" });
  }
});


