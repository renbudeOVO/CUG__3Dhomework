#include "Dhomework2.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <QString>
#include <QMessageBox>

// 构造函数
Dhomework2::Dhomework2(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);  // 连接 UI 界面

	// 当点击 "开始" 按钮时，开始dem生成赋值和压缩
	connect(ui.startpushButton, &QPushButton::clicked, this, &Dhomework2::onStartButtonClicked);
	connect(ui.cancelpushButton, &QPushButton::clicked, this, &Dhomework2::onCancelButtonClicked);
}

// 析构函数
Dhomework2::~Dhomework2() {
	// UI 对象的内存会自动释放，因此不需要手动 delete
}

// 动态生成DEM数据
void Dhomework2::generateDEM(int width, int height, int** demBuf) {
	srand(time(0));  // 使用时间作为种子生成随机数

	// 首先为第一个网格点赋值
	demBuf[0][0] = rand() % 100;

	// 填充第一行，使用较大概率使相邻点相同
	for (int j = 1; j < width; ++j) {
		if (rand() % 100 < 80) {  // 80% 概率继承前一个值
			demBuf[0][j] = demBuf[0][j - 1];
		}
		else {  // 20% 概率随机生成新的值
			demBuf[0][j] = rand() % 100;
		}
	}

	// 填充剩余行，同样根据相邻点值生成
	for (int i = 1; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j == 0) {
				// 第一列，比较上一行的第一个元素
				if (rand() % 100 < 80) {
					demBuf[i][j] = demBuf[i - 1][j];
				}
				else {
					demBuf[i][j] = rand() % 100;
				}
			}
			else {
				// 其他列，既可以比较上方元素也可以比较左侧元素
				if (rand() % 100 < 80) {
					demBuf[i][j] = demBuf[i][j - 1];  // 80% 概率继承左侧值
				}
				else if (rand() % 100 < 60) {
					demBuf[i][j] = demBuf[i - 1][j];  // 60% 概率继承上方值
				}
				else {
					demBuf[i][j] = rand() % 100;  // 否则随机生成
				}
			}
		}
	}
}

// 行程编码函数
void Dhomework2::RunLength2D(int sc, int sr, int v, int n, std::vector<int>& encodedData) {
	encodedData.push_back(sc); // 行号
	encodedData.push_back(sr); // 列号
	encodedData.push_back(v);  // 值
	encodedData.push_back(n);  // 数据块的长度
}

// 行程编码过程
void Dhomework2::encodeDEM(int** demBuf, int width, int height, std::vector<int>& encodedData) {
	for (int i = 0; i < height; ++i) {
		int prevVal = demBuf[i][0];
		int count = 1;
		for (int j = 1; j < width; ++j) {
			if (demBuf[i][j] == prevVal) {
				count++;
			}
			else {
				RunLength2D(i, j - count, prevVal, count, encodedData);
				prevVal = demBuf[i][j];
				count = 1;
			}
		}
		// 处理行末未压缩的部分
		RunLength2D(i, width - count, prevVal, count, encodedData);
	}
}

// 计算压缩比并输出
void Dhomework2::calculateCompressionRatio(int originalSize, int compressedSize) {
	double ratio = static_cast<double>(originalSize) / compressedSize;
	QString result = QString("压缩比 (Ratio): %1\n").arg(ratio);
	ui.plainTextEdit_2->appendPlainText(result);
}

// 当按下 "开始" 按钮时执行此函数
void Dhomework2::onStartButtonClicked() {
	// 重置进度条的值
	ui.progressBar->setValue(0);

	// 获取宽度和高度的输入
	int width = ui.widthlineEdit->text().toInt();
	int height = ui.heightlineEdit->text().toInt();

	if (width <= 0 || height <= 0) {
		// 使用QMessageBox输出警告
		QMessageBox::warning(this, "警告", "无效的宽度或高度！");
		return;
	}

	// 动态申请内存
	int** demBuf = new int* [height];
	for (int i = 0; i < height; ++i) {
		demBuf[i] = new int[width];
	}

	// 生成随机DEM数据
	generateDEM(width, height, demBuf);
	ui.progressBar->setValue(10);

	// 获取当前文本内容
	QString output = ui.plainTextEdit_2->toPlainText();

	// 输出宽度和高度，并添加分隔符
	output.append("===================\n");
	output.append(QString("DEM 宽度: %1\n").arg(width));
	output.append(QString("DEM 高度: %1\n").arg(height));
	output.append("===================\n");

	// 输出原始DEM数据
	output.append("原始 DEM 数据:\n");
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			output.append(QString("%1 ").arg(demBuf[i][j]));
		}
		output.append("\n");
	}
	ui.progressBar->setValue(20);

	// 添加分隔符
	output.append("===================\n");

	// 编码后的数据
	std::vector<int> encodedData;

	// 执行行程编码
	encodeDEM(demBuf, width, height, encodedData);
	ui.progressBar->setValue(30);

	// 输出编码后的数据
	output.append("\n编码后的数据:\n");
	for (size_t i = 0; i < encodedData.size(); i += 4) {
		output.append(QString("行号: %1, 列号: %2, 值: %3, 长度: %4\n")
			.arg(encodedData[i])
			.arg(encodedData[i + 1])
			.arg(encodedData[i + 2])
			.arg(encodedData[i + 3]));
	}
	ui.progressBar->setValue(50);

	// 添加分隔符
	output.append("===================\n");

	// 计算原始和压缩后数据大小
	int originalSize = width * height * sizeof(int);
	int compressedSize = encodedData.size() * sizeof(int);
	ui.progressBar->setValue(70);

	// 输出原始和压缩后的数据大小
	output.append(QString("\n原始数据大小: %1 字节\n").arg(originalSize));
	output.append(QString("压缩后数据大小: %1 字节\n").arg(compressedSize));

	// 计算并输出压缩比
	double ratio = static_cast<double>(originalSize) / compressedSize;
	output.append(QString("压缩比 (Ratio): %1\n").arg(ratio));
	ui.progressBar->setValue(80);

	// 添加分隔符
	output.append("===================\n");

	// 更新文本框以展示完整数据
	ui.plainTextEdit_2->setPlainText(output);
	ui.progressBar->setValue(90);

	// 释放内存
	for (int i = 0; i < height; ++i) {
		delete[] demBuf[i];
	}
	delete[] demBuf;

	ui.progressBar->setValue(100);

	// 显示成功提示
	QMessageBox::information(this, "成功", "处理完成！");
}


void Dhomework2::onCancelButtonClicked() {
	ui.progressBar->setValue(0);
	ui.widthlineEdit->clear();
	ui.heightlineEdit->clear();
}