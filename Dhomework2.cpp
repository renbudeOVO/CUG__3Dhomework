#include "Dhomework2.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <QString>
#include <QMessageBox>

// ���캯��
Dhomework2::Dhomework2(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);  // ���� UI ����

	// ����� "��ʼ" ��ťʱ����ʼdem���ɸ�ֵ��ѹ��
	connect(ui.startpushButton, &QPushButton::clicked, this, &Dhomework2::onStartButtonClicked);
	connect(ui.cancelpushButton, &QPushButton::clicked, this, &Dhomework2::onCancelButtonClicked);
}

// ��������
Dhomework2::~Dhomework2() {
	// UI ������ڴ���Զ��ͷţ���˲���Ҫ�ֶ� delete
}

// ��̬����DEM����
void Dhomework2::generateDEM(int width, int height, int** demBuf) {
	srand(time(0));  // ʹ��ʱ����Ϊ�������������

	// ����Ϊ��һ������㸳ֵ
	demBuf[0][0] = rand() % 100;

	// ����һ�У�ʹ�ýϴ����ʹ���ڵ���ͬ
	for (int j = 1; j < width; ++j) {
		if (rand() % 100 < 80) {  // 80% ���ʼ̳�ǰһ��ֵ
			demBuf[0][j] = demBuf[0][j - 1];
		}
		else {  // 20% ������������µ�ֵ
			demBuf[0][j] = rand() % 100;
		}
	}

	// ���ʣ���У�ͬ���������ڵ�ֵ����
	for (int i = 1; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j == 0) {
				// ��һ�У��Ƚ���һ�еĵ�һ��Ԫ��
				if (rand() % 100 < 80) {
					demBuf[i][j] = demBuf[i - 1][j];
				}
				else {
					demBuf[i][j] = rand() % 100;
				}
			}
			else {
				// �����У��ȿ��ԱȽ��Ϸ�Ԫ��Ҳ���ԱȽ����Ԫ��
				if (rand() % 100 < 80) {
					demBuf[i][j] = demBuf[i][j - 1];  // 80% ���ʼ̳����ֵ
				}
				else if (rand() % 100 < 60) {
					demBuf[i][j] = demBuf[i - 1][j];  // 60% ���ʼ̳��Ϸ�ֵ
				}
				else {
					demBuf[i][j] = rand() % 100;  // �����������
				}
			}
		}
	}
}

// �г̱��뺯��
void Dhomework2::RunLength2D(int sc, int sr, int v, int n, std::vector<int>& encodedData) {
	encodedData.push_back(sc); // �к�
	encodedData.push_back(sr); // �к�
	encodedData.push_back(v);  // ֵ
	encodedData.push_back(n);  // ���ݿ�ĳ���
}

// �г̱������
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
		// ������ĩδѹ���Ĳ���
		RunLength2D(i, width - count, prevVal, count, encodedData);
	}
}

// ����ѹ���Ȳ����
void Dhomework2::calculateCompressionRatio(int originalSize, int compressedSize) {
	double ratio = static_cast<double>(originalSize) / compressedSize;
	QString result = QString("ѹ���� (Ratio): %1\n").arg(ratio);
	ui.plainTextEdit_2->appendPlainText(result);
}

// ������ "��ʼ" ��ťʱִ�д˺���
void Dhomework2::onStartButtonClicked() {
	// ���ý�������ֵ
	ui.progressBar->setValue(0);

	// ��ȡ��Ⱥ͸߶ȵ�����
	int width = ui.widthlineEdit->text().toInt();
	int height = ui.heightlineEdit->text().toInt();

	if (width <= 0 || height <= 0) {
		// ʹ��QMessageBox�������
		QMessageBox::warning(this, "����", "��Ч�Ŀ�Ȼ�߶ȣ�");
		return;
	}

	// ��̬�����ڴ�
	int** demBuf = new int* [height];
	for (int i = 0; i < height; ++i) {
		demBuf[i] = new int[width];
	}

	// �������DEM����
	generateDEM(width, height, demBuf);
	ui.progressBar->setValue(10);

	// ��ȡ��ǰ�ı�����
	QString output = ui.plainTextEdit_2->toPlainText();

	// �����Ⱥ͸߶ȣ�����ӷָ���
	output.append("===================\n");
	output.append(QString("DEM ���: %1\n").arg(width));
	output.append(QString("DEM �߶�: %1\n").arg(height));
	output.append("===================\n");

	// ���ԭʼDEM����
	output.append("ԭʼ DEM ����:\n");
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			output.append(QString("%1 ").arg(demBuf[i][j]));
		}
		output.append("\n");
	}
	ui.progressBar->setValue(20);

	// ��ӷָ���
	output.append("===================\n");

	// ����������
	std::vector<int> encodedData;

	// ִ���г̱���
	encodeDEM(demBuf, width, height, encodedData);
	ui.progressBar->setValue(30);

	// �������������
	output.append("\n����������:\n");
	for (size_t i = 0; i < encodedData.size(); i += 4) {
		output.append(QString("�к�: %1, �к�: %2, ֵ: %3, ����: %4\n")
			.arg(encodedData[i])
			.arg(encodedData[i + 1])
			.arg(encodedData[i + 2])
			.arg(encodedData[i + 3]));
	}
	ui.progressBar->setValue(50);

	// ��ӷָ���
	output.append("===================\n");

	// ����ԭʼ��ѹ�������ݴ�С
	int originalSize = width * height * sizeof(int);
	int compressedSize = encodedData.size() * sizeof(int);
	ui.progressBar->setValue(70);

	// ���ԭʼ��ѹ��������ݴ�С
	output.append(QString("\nԭʼ���ݴ�С: %1 �ֽ�\n").arg(originalSize));
	output.append(QString("ѹ�������ݴ�С: %1 �ֽ�\n").arg(compressedSize));

	// ���㲢���ѹ����
	double ratio = static_cast<double>(originalSize) / compressedSize;
	output.append(QString("ѹ���� (Ratio): %1\n").arg(ratio));
	ui.progressBar->setValue(80);

	// ��ӷָ���
	output.append("===================\n");

	// �����ı�����չʾ��������
	ui.plainTextEdit_2->setPlainText(output);
	ui.progressBar->setValue(90);

	// �ͷ��ڴ�
	for (int i = 0; i < height; ++i) {
		delete[] demBuf[i];
	}
	delete[] demBuf;

	ui.progressBar->setValue(100);

	// ��ʾ�ɹ���ʾ
	QMessageBox::information(this, "�ɹ�", "������ɣ�");
}


void Dhomework2::onCancelButtonClicked() {
	ui.progressBar->setValue(0);
	ui.widthlineEdit->clear();
	ui.heightlineEdit->clear();
}