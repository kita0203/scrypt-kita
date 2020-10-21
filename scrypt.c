//N: メモリ使用量および計算量を調整するためのパラメータ。
//r: メモリ使用量を調整するためのパラメータ。
//p: 計算量を調整するためのパラメータ。
//k: scryptの処理単位である「ブロック」の大きさ（ビット単位）。scryptの実装では512ビット=64バイト。

//HASH(data): kビットの出力を持つ簡易なハッシュ関数。scryptの実装ではSalsa20/8が用いられる。
//AS_LE(data): 入力されたバイト列をリトルエンディアンの整数として解釈し、返却する。
//PBKDF2(password, salt, iteration, output_length_in_bits): scryptの実装ではPBKDF2+SHA256を用いている。

// 入力されたデータを簡易的なハッシュ関数に通しながら撹拌し、入力と全く同じ長さのデータを出力する。
// Input:
//   Z: kビット✕2r個の配列。
// Output:
//   kビット✕2r個の配列。
void salsa(){

}
void PBKDF2(password,salt,output){

}
void BlockMix(Z[0..2r]){
  // 作業用のkビットのデータを初期化。
  X := Z[2r-1]
  // Xの値を順次アップデートしながら、その値を2r個の出力Yとする。
  for j in (0..2r-1):
    X &lt;= HASH(X xor Z[j])
    Y[j] := X
  // Yのデータを適切に並び替えて最終結果とする。
  return (Y[0] || Y[2] || ... || Y[2r-2]) || (Y[1] || Y[3] || ... || Y[2r-1])
}
// scryptのコア・ルーチン。大量のメモリを作業領域として用いながら入力データを撹拌し、入力と全く同じ長さのデータを出力する。
// Input:
//   Bi: kビット✕2r個の配列。
// Output:
//   kビット✕2r個の配列。
void ROMix(B[i],N){
  // BiにBlockMixをj回適用したものをV[j]とし、N個の配列Vを初期化する。
  for j in (0..N-1):
    V[j][0..2r-1] = BlockMix^j(Bi[0..2r-1])
  // BiにBlockMixをN回適用したものをXとして初期化。
  X[0..2r-1] := BlockMix^N(Bi[0..2r-1])
  // N回以下の処理を実行
  for N times:
    // X[2r-1]をリトルエンディアン(LE)の整数と解釈し、Nで割った余りをkとする。kは擬似乱数として振る舞う。
    k &lt;= AS_LE(X[2r-1]) mod N
    // XおよびV[k]の排他的論理和をとり、BlockMixする。kのランダム性より、ランダムな場所からVのデータを読みだす必要がある。
    X[0..2r-1] &lt;= BlockMix(X[0..2r-1] xor V[k][0..2r-1])
  return X[0..2r-1]
}
// 入力された配列Bをシリアライズし、ひとつのバイト列として返却する。
// Input:
//   B: kビット✕p個✕2r個の配列。
// Output:
//   k✕p✕2rビットのバイト列。
function serialize(B[0..p-1][0..2r-1]):
  return (B[0][0]) || (B[0][1]) || ... || (B[0][2r-1]) || (B[1][0]) || (B[1][1]) || ... || (B[p-1][r-1])

// 与えられたパスワードおよびsaltからscryptハッシュ値を計算する。
// Input:
//   password: ユーザの入力したパスワードのバイト列。
//   salt: saltのバイト列。
//   outlen: 出力されるハッシュ値の長さ。
// Output:
//   outlenビットのバイト列（ハッシュ値）。
int main(int argc, char *argv[]){
    char salt,password;
    int outlen,r,i,p,N;
    int output_length_in_bits;

    password=argv[1];
    salt=argv[2];
    outlen=argv[3];
    r=argv[4];
    p=argv[5];
    N=argv[6];

    int B[p];
  // PBKDF2を使い、入力のパスワードおよびsaltからkビット×p個×2r個の大きさを持つ配列Bを初期化。
  for(i = 0; i < argv[5]; i++){
      (B[i]) = PBKDF2(password, salt, argv[3]);
  }   
  // scryptのコア・ルーチンであるROMixを各B[i]に適用していく。この処理は自明に並列化可能である。
  for(i=0;i<p;i++){
       ROMix(B[i],N);
  }
 
  // B を salt とし、PBKDF2 を用いて最終結果を計算する。
  return PBKDF2(password, salt=serialize(B), iteration=1, output_length_in_bits=outlen);
}
