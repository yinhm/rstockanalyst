#include <QApplication>
#include <iostream>
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QString qsSrcFile = app.applicationDirPath()+"/1.txt";
	QFile fileSrc(qsSrcFile);			//加载绘制文件

	if(!fileSrc.open(QFile::ReadOnly))		//打开文件
	{
		std::cout<<"open src file '"<<qsSrcFile.toStdString()<<"' error."<<std::endl;
		return -1;							//失败退出
	}


	QImage image(500,500,QImage::Format_ARGB32);			//创建一个500*500的画布
	QPainter painter(&image);								//将画布与QPainter相关联
	painter.setRenderHint(QPainter::Antialiasing,true);		//设置抗锯齿，主要用来防止直线锯齿

	QString qsLine = fileSrc.readLine().trimmed();			//读取第一行数据

	QVector<QLineF> listLines;

	QPointF ptFront;
	{
		//读取初始点
		QStringList listPos = qsLine.split(",\t");
		if(listPos.size()>1)
			ptFront = QPointF((listPos[0].toFloat()+5.0)*50.0,(listPos[1].toFloat())*50.0);
		else
		{
			std::cout<<"read first point error."<<std::endl;
			return -1;
		}
	}

	//读取下一个点
	qsLine = fileSrc.readLine().trimmed();

	while(!qsLine.isEmpty())
	{
		QStringList listPos = qsLine.split(",\t");
		if(listPos.size()>1)
		{
			QPointF ptNow = QPointF((listPos[0].toFloat()+5.0)*50.0,(listPos[1].toFloat())*50.0);
			listLines.push_back(QLineF(ptFront,ptNow));
			ptFront = ptNow;
		}
		else
		{
			std::cout<<"read point error."<<std::endl;
			return -1;
		}

		qsLine = fileSrc.readLine().trimmed();
	}

	painter.drawLines(listLines);			//绘制直线


	//将绘制完的图片显示在一个QLabel上。
	{
		//第一种显示方式，直接将label显示出来
		QLabel label(0);
		label.setPixmap(QPixmap::fromImage(image));
		label.show();
		return app.exec();
	}
/*	{
		//将lable放入widget中，再显示出来。
		QWidget widget(0);
		QLabel label(&widget);
		label.setPixmap(image);
		QVBoxLayout layout(&widget);
		layout.addWidget(&label);
		widget.setLayout(&layout);
		widget.show();
		return app.exec();
	}
*/
}
