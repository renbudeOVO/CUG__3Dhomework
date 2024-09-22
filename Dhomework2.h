#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Dhomework2.h"  // �������ɵ� UI ��ͷ�ļ�


//��ֹqt������������
#ifdef WIN32
#pragma  execution_character_set("utf-8")
#endif

class Dhomework2 : public QMainWindow
{
	Q_OBJECT

public:
	// ���캯������������
	Dhomework2(QWidget* parent = nullptr);
	~Dhomework2();

private slots:
	// �ۺ�����������Ӧ��ť����¼�
	void onStartButtonClicked();
	void onCancelButtonClicked();
private:
	// ���� UI ����
	Ui::Dhomework2Class ui;

	// ������Ա����
	void generateDEM(int width, int height, int** demBuf);
	void encodeDEM(int** demBuf, int width, int height, std::vector<int>& encodedData);
	void RunLength2D(int sc, int sr, int v, int n, std::vector<int>& encodedData);
	void calculateCompressionRatio(int originalSize, int compressedSize);
};
