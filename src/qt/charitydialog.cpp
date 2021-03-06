#include "charitydialog.h"
#include "ui_charitydialog.h"

#include "walletmodel.h"
#include "base58.h"
#include "addressbookpage.h"
#include "init.h"

#include <QMessageBox>

StakeForCharityDialog::StakeForCharityDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StakeForCharityDialog),
	 model(0)
{
    ui->setupUi(this);
}

StakeForCharityDialog::~StakeForCharityDialog()
{
    delete ui;
}

void StakeForCharityDialog::setModel(WalletModel *model)
{
	this->model = model;
	
	QString strAddress = model->getStakeForCharityAddress();
	QString strPer = QString::number(model->getStakeForCharityPercent());
	
	if (!strAddress.isEmpty() && strPer.toInt() > 0 )
	{
		ui->charityAddressEdit->setText(strAddress);
		ui->charityPercentEdit->setText(strPer);
		ui->message->setStyleSheet("QLabel { color: green; }");
		ui->message->setText(tr("You are now saving to\n") + strAddress + tr("."));
	}
}

void StakeForCharityDialog::setAddress(const QString &address)
{
	ui->charityAddressEdit->setText(address);
	ui->charityAddressEdit->setFocus();
}

void StakeForCharityDialog::on_addressBookButton_clicked()
{
    if (model && model->getAddressTableModel())
    {
        AddressBookPage dlg(AddressBookPage::ForSending, AddressBookPage::SendingTab, this);
        dlg.setModel(model->getAddressTableModel());
        if (dlg.exec())
        {
            setAddress(dlg.getReturnValue());
        }
    }
}
/*
void StakeForCharityDialog::on_buttonBox_accepted()
{
    QMessageBox msgBox;
    CBitcoinAddress address = ui->charityAddressEdit->text().toStdString();
    QString str = ui->charityPercentEdit->text();
    bool fIntConversion;
    unsigned int nCharityPercent = str.toInt(&fIntConversion, 10);


                   if (!address.IsValid())
                   {
                       msgBox.setText("Invalid HyperStake address");
                       msgBox.exec();
                       return;
                   }
                   if (nCharityPercent < 1)
                   {
                        msgBox.setText("Invalid parameter, expected valid percentage");
                        msgBox.exec();
                        return;
                   }
                   if (pwalletMain->IsLocked())
                   {
                       msgBox.setText("Error: Please enter the wallet passphrase with walletpassphrase first.");
                       msgBox.exec();
                       return;
                   }

                   //Turn off if we set to zero.
                   //Future: After we allow multiple addresses, only turn of this address
                   if(nCharityPercent == 0)
                   {
                       pwalletMain->fStakeForCharity = false;
                       pwalletMain->StakeForCharityAddress = "";
                       pwalletMain->nStakeForCharityPercent = 0;

                       msgBox.setText("0 Percent Selected, void");
                       msgBox.exec();
                       return;
                   }


                   //For now max percentage is 50.
                   if (nCharityPercent > 50 )
                      nCharityPercent = 50;

                   pwalletMain->StakeForCharityAddress = address;
                   pwalletMain->nStakeForCharityPercent = nCharityPercent;
                   pwalletMain->fStakeForCharity = true;

                   msgBox.setText("Stake For Charity Set");
                   msgBox.exec();
                   close();
}*/

void StakeForCharityDialog::on_enableButton_clicked()
{
    if(model->getEncryptionStatus() == WalletModel::Locked)
    {
        ui->message->setStyleSheet("QLabel { color: black; }");
        ui->message->setText(tr("Please unlock wallet before starting auto savings."));
        return;
    }

    bool fValidConversion = false;
    int64 nMinAmount = MIN_TXOUT_AMOUNT;
    int64 nMaxAmount = MAX_MONEY;

    CBitcoinAddress address = ui->charityAddressEdit->text().toStdString();
    if (!address.IsValid())
    {
        ui->message->setStyleSheet("QLabel { color: red; }");
        ui->message->setText(tr("The entered address: ") + ui->charityAddressEdit->text() + tr(" is invalid.\nPlease check the address and try again."));
        ui->charityAddressEdit->setFocus();
        return;
    }

    int nCharityPercent = ui->charityPercentEdit->text().toInt(&fValidConversion, 10);
    if (!fValidConversion || nCharityPercent > 50 || nCharityPercent <= 0)
    {
        ui->message->setStyleSheet("QLabel { color: red; }");
        ui->message->setText(tr("Please Enter 1 - 50 for percent."));
        ui->charityPercentEdit->setFocus();
        return;
    }

    if (!ui->charityMinEdit->text().isEmpty())
    {
        nMinAmount = ui->charityMinEdit->text().toDouble(&fValidConversion) * COIN;
        if(!fValidConversion || nMinAmount <= MIN_TXOUT_AMOUNT || nMinAmount >= MAX_MONEY  )
        {
            ui->message->setStyleSheet("QLabel { color: red; }");
            ui->message->setText(tr("Min Amount out of Range, please re-enter."));
            ui->charityMinEdit->setFocus();
            return;
        }
    }

    if (!ui->charityMaxEdit->text().isEmpty())
    {
        nMaxAmount = ui->charityMaxEdit->text().toDouble(&fValidConversion) * COIN;
        if(!fValidConversion || nMaxAmount <= MIN_TXOUT_AMOUNT || nMaxAmount >= MAX_MONEY  )
        {
            ui->message->setStyleSheet("QLabel { color: red; }");
            ui->message->setText(tr("Max Amount out of Range, please re-enter."));
            ui->charityMaxEdit->setFocus();
            return;
        }
    }

    model->setStakeForCharity(true, nCharityPercent, address, nMinAmount, nMaxAmount);
    ui->message->setStyleSheet("QLabel { color: green; }");
    ui->message->setText(tr("You are now sending to\n") + QString(address.ToString().c_str()) + tr("."));
    return;
}

void StakeForCharityDialog::on_disableButton_clicked()
{
    int nCharityPercent = 0;
    CBitcoinAddress address = "";
    int64 nMinAmount = MIN_TXOUT_AMOUNT;
    int64 nMaxAmount = MAX_MONEY;

    model->setStakeForCharity(false, nCharityPercent, address, nMinAmount, nMaxAmount);
    ui->charityAddressEdit->clear();
    ui->charityMaxEdit->clear();
    ui->charityMinEdit->clear();
    ui->charityPercentEdit->clear();
    ui->message->setStyleSheet("QLabel { color: black; }");
    ui->message->setText(tr("Auto Savings is now off"));
    return;
}

/*
void StakeForCharityDialog::on_pushButton_clicked()
{
    QMessageBox msgBox;
    pwalletMain->fStakeForCharity = false;
    pwalletMain->StakeForCharityAddress = "";
    pwalletMain->nStakeForCharityPercent = 0;

    msgBox.setText("Stake For Charity Disabled");
    msgBox.exec();

}*/
