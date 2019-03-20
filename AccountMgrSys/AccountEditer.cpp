#include "AccountEditer.h"
#include "CommentData.h"
#include "SQLiteHelper.h"
#include <QSqlTableModel>
#include "ItemDelegate.h"

AccountEditer::AccountEditer(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.radioButton->setChecked(true);
	connect(ui.lineEdit, &QLineEdit::textChanged, this, &AccountEditer::slotqueryAccount);
	connect(ui.pushButton, &QPushButton::clicked, this, &AccountEditer::slotAddAccount);
	connect(ui.pushButton_3, &QPushButton::clicked, this, &AccountEditer::slotqueryAccount);
	connect(ui.pushButton_4, &QPushButton::clicked, this, &AccountEditer::slotDeleteAccount);
	connect(ui.pushButton_5, &QPushButton::clicked, this, &AccountEditer::slotSubmitBtnClicked);
	QSqlTableModel *pModel = new QSqlTableModel(this, gDataBaseMgr->getAccountDB());
	pModel->setTable("Account");
	// �����ύ��ʽ���Ķ����ύ��ѡ��������ʱ�ύ���ֶ��ύ����ʵ�ʲ��ԣ�����ֻ���ֶ��ύ����ʾЧ��������õ�
	pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	pModel->select();
	pModel->setHeaderData(0, Qt::Horizontal, "����ԱID");
	pModel->setHeaderData(1, Qt::Horizontal, "����Ա��");
	pModel->setHeaderData(2, Qt::Horizontal, "�Ա�");
	pModel->setHeaderData(3, Qt::Horizontal, "����Ա����");
	pModel->setHeaderData(4, Qt::Horizontal, "����֤��");
	pModel->setHeaderData(5, Qt::Horizontal, "��¼����");
	pModel->setHeaderData(6, Qt::Horizontal, "��ϵ�绰");
	pModel->setHeaderData(7, Qt::Horizontal, "��ַ");
	pModel->setHeaderData(8, Qt::Horizontal, "����");
	pModel->sort(0, Qt::AscendingOrder); // ��0����������
	ui.tableView->setModel(pModel);
	ui.tableView->setShowGrid(false);
	ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //�п�����Ӧ
	ui.tableView->horizontalHeader()->setMinimumSectionSize(50); 
	ui.tableView->horizontalHeader()->setStretchLastSection(true); //���һ���Զ���ȫ
	ui.tableView->horizontalHeader()->setSortIndicatorShown(true); //����С��ͷ
	
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);// ֻ�ܵ�ѡ
	
	ui.tableView->setSelectionBehavior(QAbstractItemView::QAbstractItemView::SelectRows);// ������Ϊѡ���׼

	//ui.tableView->verticalHeader()->hide();	// ��ͷ����
	
	ui.tableView->horizontalHeader()->setSectionsClickable(true);// ����ͷ�ɱ��������������¼�
	
	ui.tableView->horizontalHeader()->setHighlightSections(false);// ȥ��ѡ�б���ʱ����ͷ�����ָ���
	ui.tableView->horizontalHeader()->setBackgroundRole(QPalette::Background);

	//ui.tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(225,225,225)};");// ��ͷ��ɫ
	connect(ui.tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));

	AccountEditorDelegate *_sexDelegate = new AccountEditorDelegate;
	ui.tableView->setItemDelegateForColumn(2, _sexDelegate);
	AccountEditorDelegate *_sexDelegate2 = new AccountEditorDelegate;
	ui.tableView->setItemDelegateForColumn(3, _sexDelegate2);

}

AccountEditer::~AccountEditer()
{
}

void AccountEditer::updateAccountList()
{

}

void AccountEditer::slotAddAccount()
{
	QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(ui.tableView->model());
	AccountAddDialog *_accountAddDialog = new AccountAddDialog;
	if (_accountAddDialog->exec())
		pMode->submitAll();
}

void AccountEditer::slotDeleteAccount()
{
	QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(ui.tableView->model());
	QSqlRecord _record = pMode->record(ui.tableView->currentIndex().row());
	int ret = QMessageBox::warning(this, tr("����"), tr("�Ƿ�ɾ��:%1��").arg(_record.value("Name").toString()), QMessageBox::Ok | QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel)
		return;
	pMode->removeRow(ui.tableView->currentIndex().row());
	// ÿ���ֶ��ύ����������ˢ��tableView������mode��tableViewһ��
	pMode->submitAll();
}

void AccountEditer::slotSubmitBtnClicked()
{
	QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(ui.tableView->model());
	pMode->database().transaction(); //��ʼ�������
	if (pMode->submitAll()) // �ύ���б��޸ĵ����ݵ����ݿ���
	{
		pMode->database().commit(); //�ύ�ɹ������������޸����ݿ�����
		QMessageBox::information(this, tr("��ʾ"), tr("�����ύ�ɹ���"));
	}
	else {
		pMode->database().rollback(); //�ύʧ�ܣ�����ع�
		QMessageBox::warning(this, tr("tableModel"), tr("���ݿ����: %1").arg(pMode->lastError().text()));
	}
	pMode->revertAll(); //�����޸�
}

void AccountEditer::slotqueryAccount()
{
	QString name = ui.lineEdit->text();
	//ɸѡ
	if (!ui.lineEdit->text().isEmpty())
		//dynamic_cast<QSqlTableModel *>(ui.tableView->model())->setFilter(QString("(Name Like '%%1%') OR (ID Like '%%2%')").arg(name).arg(name));
		dynamic_cast<QSqlTableModel *>(ui.tableView->model())->setFilter(QString("Name Like '%%1%'").arg(name));
	if (ui.lineEdit->text().isEmpty())
	{
		dynamic_cast<QSqlTableModel *>(ui.tableView->model())->setFilter(QString("Name Like '%%'"));
	}
	//��ѯ
	dynamic_cast<QSqlTableModel *>(ui.tableView->model())->select();
}

void AccountEditer::sortByColumn(int col)
{
	QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(ui.tableView->model());
	bool ascending = (ui.tableView->horizontalHeader()->sortIndicatorSection() == col
		&& ui.tableView->horizontalHeader()->sortIndicatorOrder() == Qt::DescendingOrder);
	Qt::SortOrder order = ascending ? Qt::AscendingOrder : Qt::DescendingOrder;
	pMode->sort(col, order);
}