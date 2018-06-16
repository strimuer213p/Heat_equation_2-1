#include<iostream>
#include<vector>

static double k = 0.467; //牛肉の熱伝導率(物性値より定数)  参考文献　https://www.jstage.jst.go.jp/article/nskkk1995/43/4/43_4_368/_pdf　3ページ目
static double a = 0.00000013; //牛肉の熱拡散率(物性値より定数)　参考文献　https://www.jstage.jst.go.jp/article/jhej1987/50/2/50_2_147/_pdf　3ページ目
static double h = 20;//空気の対流熱伝達率
static double dx = 0.002;//空間刻み
static double dy = 0.002;//空間刻み
static double dt = 2;//時間刻み
static double lim_t = 1000;//最大時間
static double x0_ = 200;//x=0(x側の鉄板の温度)
static double y0_ = 200;//y=0(y側の鉄板の温度)　//なんか現実問題に置き換えた時ｘは鉄板だとして、yが壁なんですが鉄板と壁にくっついてる状態って変な感じなので後でなんか変えます
static double air_T = 25;//空気の温度
static double meat_T = 20;//初期の肉の温度

int main() {

	double x;//横　//安直ですが僕がWidthやHeightだと直感で判りずらいので(z拡張も含め)
	double y;//縦　

	std::cout << "縦の長さを入力してください[cm]" << std::endl;
	std::cin >> y;
	std::cout << "横の長さを入力してください[cm]" << std::endl;
	std::cin >> x;

	x = x / 100.0;//肉の厚さを[m]に変換
	y = y / 100.0;

	int t_num = int(lim_t / dt);//時間分割数
	int x_num = int(x / dx) + 2; //空間分割数 鉄板をx=0 x=[max]を空気とするため+2
	int y_num = int(y / dy) + 2; //空間分割数 鉄板をy=0 y=[max]を空気とするため+2

	std::vector<std::vector<std::vector<double>>> T; //T[時間t][場所x][場所y]
	T = std::vector<std::vector<std::vector<double>>>(t_num, std::vector<std::vector<double>>(x_num, std::vector<double>(y_num, meat_T))); //全ての時間の温度と場所を初期の肉の温度で初期化

	std::vector<std::vector<std::vector<double>>> T_(T);//コピーコンストラクタ　行列 T と全く同じもの(斜めの計算のためにもう一つ格納する行列を作りたかった)


	for (int t = 1; t < t_num; t++) {
		for (int x = 2; x < x_num; x++) {
			for (int y = 2; y < y_num; y++) {
				T[t - 1][0][y - 1] = x0_;//xについての第1種境界条件
				T[t - 1][x - 1][0] = y0_;//yについての第1種境界条件
				T[t - 1][x_num - 1][y] = h * (air_T - T[t - 1][x_num - 2][y]); //xについての第3種境界条件
				T[t - 1][x][y_num - 1] = h * (air_T - T[t - 1][x][y_num - 2]); //xについての第3種境界条件

																			   //熱伝導方程式
																			   //上下左右の４つを考慮した場合のみ　斜めの熱の動きは計算してない
				T[t][x - 1][y - 1] = a * dt*(T[t - 1][x][y - 1] + T[t - 1][x - 2][y - 1] + T[t - 1][x - 1][y] + T[t - 1][x - 1][y - 2] - 4 * T[t - 1][x - 1][y - 1]) / dx / dx + T[t - 1][x - 1][y - 1]; //拡散方程式(熱伝導方程式)　      とりあえず普通の差分法(オイラーやルンゲクッタでもっと正確にできそう(？))

				T_[t - 1][0][y - 1] = x0_;//xについての第1種境界条件
				T_[t - 1][x - 1][0] = y0_;//yについての第1種境界条件
				T_[t - 1][x_num - 1][y] = h * (air_T - T[t - 1][x_num - 2][y]); //xについての第3種境界条件
				T_[t - 1][x][y_num - 1] = h * (air_T - T[t - 1][x][y_num - 2]); //xについての第3種境界条件

																				//熱伝導方程式
																				//上下左右と各方向の斜めの8つを考慮した場合
				T_[t][x - 1][y - 1] = a * dt*(T_[t - 1][x][y] + T_[t - 1][x][y - 1] + T_[t - 1][x][y - 2] + +T_[t - 1][x - 1][y] + T_[t - 1][x - 1][y - 2] + T_[t - 1][x - 2][y] + T_[t - 1][x - 2][y - 1] + T_[t - 1][x - 2][y - 2] - 8 * T_[t - 1][x - 1][y - 1]) / dx / dx + T_[t - 1][x - 1][y - 1]; //拡散方程式(熱伝導方程式)　      とりあえず普通の差分法(オイラーやルンゲクッタでもっと正確にできそう(？))

			}
		}
	}



	double time = 0;

	for (int i = 0; i < t_num; i++) {  //ここからTの出力
		std::cout << time << "[s] " << std::endl; //実時間の出力


		for (int j = 0; j < x_num - 1; j++) {
			for (int k = 0; k < y_num - 1; k++) {

				std::cout << T[i][j][k] << " ";  //すべての時間とすべての場所が記録されている2次元行列　T[時間][場所]　の結果出力		
			}
			std::cout << std::endl;  //1列ごとに改行

		}
		std::cout << std::endl;  //1列ごとに改行
		time = time + dt; //時間を実時間に直して出力
	}

	time = 0;

	for (int i = 0; i < t_num; i++) {  //ここからT_の出力
		std::cout << time << "[s] " << std::endl; //実時間の出力

		for (int j = 0; j < x_num - 1; j++) {
			for (int k = 0; k < y_num - 1; k++) {

				std::cout << T_[i][j][k] << " ";  //すべての時間とすべての場所が記録されている2次元行列　T[時間][場所]　の結果出力		
			}
			std::cout << std::endl;  //1列ごとに改行

		}
		time = time + dt; //時間を実時間に直して出力
	}

	return 0;
}