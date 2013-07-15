#include "ipvideofeedsettingswidget.h"
#include "ui_ipvideofeedsettingswidget.h"

IpVideoFeedSettingsWidget::IpVideoFeedSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IpVideoFeedSettingsWidget)
{
    ui->setupUi(this);
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
}

IpVideoFeed *IpVideoFeedSettingsWidget::getIpVideoFeed()
{
    m_ipVideoFeed->setname(ui->lineEdit->text());
    m_ipVideoFeed->seturl(QUrl(ui->lineEdit_2->text()));

    return m_ipVideoFeed;
}
