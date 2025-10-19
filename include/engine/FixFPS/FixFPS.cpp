#include "FixFPS/FixFPS.h"
#include <thread>

FixFPS::FixFPS()
{
	previousTime_ = std::chrono::steady_clock::now();
	deltaTime_ = 0.0f;
}

void FixFPS::UpdateFixFPS()
{
	// {targetFPS_(基本60) / 1.0 秒} をマイクロ秒に変換 → 1フレーム = kFrame
    const std::chrono::microseconds kFrame = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(1.0 / targetFPS_));

    // 現在の時間を取得
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    // 経過時間を計算
    std::chrono::microseconds elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime_);

    // 目標フレーム時間に満たないなら待機
    if (elapsedTime < kFrame)
    {
		// 目標時間
        const auto target = previousTime_ + kFrame;

        // 粗いsleepで大部分を待機（最後のわずかな時間はbusy-waitで詰める）
        for (;;)
        {
			// 現在の時間を取得
            currentTime = std::chrono::steady_clock::now();
			// 残り時間を計算
            auto remain = std::chrono::duration_cast<std::chrono::microseconds>(target - currentTime);
			// 残り時間が200us以下なら抜ける
            if (remain <= std::chrono::microseconds(200)) break; // 200usだけbusy-wait
			// 200us以上ならsleep
			std::this_thread::sleep_for(remain - std::chrono::microseconds(200));
        }
		// 正確に目標時間まで待機
        while ((currentTime = std::chrono::steady_clock::now()) < target) { /* busy-wait */ }
    }
    else
    {
        currentTime = std::chrono::steady_clock::now(); // 重いフレームのときも最新時刻に更新
    }

    // 待機を含めた実フレーム時間で確定
    deltaTime_ = std::chrono::duration<float>(currentTime - previousTime_).count();
	// 前回時間を更新
    previousTime_ = currentTime;
}