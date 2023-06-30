#pragma once
#include <cstdint>
#include<xaudio2.h>
//チャンクの基本構造部分
struct Chunk{
	char id[4];//チャンクごとのid
	int32_t size;//チャンクサイズ
};
struct RiffChunk {
	Chunk chunk;//RiffChunk
	char type[4];//ファイルタイプ
};
struct FormatChunk {
	Chunk chunk;//FormatChunk
	WAVEFORMAT fmt;//波形フォーマット
};