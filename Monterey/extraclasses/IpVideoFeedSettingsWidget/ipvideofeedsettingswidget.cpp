#include "ipvideofeedsettingswidget.h"
#include "ui_ipvideofeedsettingswidget.h"

IpVideoFeedSettingsWidget::IpVideoFeedSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IpVideoFeedSettingsWidget)
{
    ui->setupUi(this);
    connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(onAutoGenerateClicked()));
}

IpVideoFeedSettingsWidget::~IpVideoFeedSettingsWidget()
{
    delete ui;
}

void IpVideoFeedSettingsWidget::setIpVideoFeed(IpVideoFeed *ipVideoFeed)
{
    m_ipVideoFeed = ipVideoFeed;
    ui->lineEdit->setText(m_ipVideoFeed->name());
    ui->lineEdit_2->setText(m_ipVideoFeed->url().toString());
    ui->checkBox->setChecked(m_ipVideoFeed->autoGenerate());
    if(ui->checkBox->isChecked())
        ui->lineEdit_2->setDisabled(true);
}

IpVideoFeed *IpVideoFeedSettingsWidget::getIpVideoFeed()
{
    m_ipVideoFeed->setname(ui->lineEdit->text());
    m_ipVideoFeed->seturl(QUrl(ui->lineEdit_2->text()));
    m_ipVideoFeed->setAutoGenerate(ui->checkBox->isChecked());

    return m_ipVideoFeed;
}

void IpVideoFeedSettingsWidget::setUrl(QString url)
{
     ui->lineEdit_2->setText(url);
}

void IpVideoFeedSettingsWidget::enableUrlEditing()
{
    ui->lineEdit_2->setEnabled(true);
}

void IpVideoFeedSettingsWidget::disableUrlEditing()
{
    ui->lineEdit_2->setEnabled(false);
}

void IpVideoFeedSettingsWidget::onAutoGenerateClicked()
{
    emit autoGenerateClicked(ui->checkBox->isChecked());
}
