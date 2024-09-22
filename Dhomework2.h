#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Dhomework2.h"  // 包含生成的 UI 类头文件


//防止qt出现中文乱码
#ifdef WIN32
#pragma  execution_character_set("utf-8")
#endif

class Dhomework2 : public QMainWindow
{
	Q_OBJECT

public:
	// 构造函数和析构函数
	Dhomework2(QWidget* parent = nullptr);
	~Dhomework2();

private slots:
	// 槽函数，用于响应按钮点击事件
	void onStartButtonClicked();
	void onCancelButtonClicked();
private:
	// 定义 UI 对象
	Ui::Dhomework2Class ui;

	// 其他成员函数
	void generateDEM(int width, int height, int** demBuf);
	void encodeDEM(int** demBuf, int width, int height, std::vector<int>& encodedData);
	void RunLength2D(int sc, int sr, int v, int n, std::vector<int>& encodedData);
	void calculateCompressionRatio(int originalSize, int compressedSize);
};
