#ifndef IPVIDEOFEEDSETTINGSWIDGET_H
#define IPVIDEOFEEDSETTINGSWIDGET_H

#include <QWidget>
#include "../../extraclasses/IpVideoFeed/ipvideofeed.h"

namespace Ui {
class IpVideoFeedSettingsWidget;
}

class IpVideoFeedSettingsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit IpVideoFeedSettingsWidget(QWidget *parent = 0);
    ~IpVideoFeedSettingsWidget();

    void setIpVideoFeed(IpVideoFeed* ipVideoFeed);
    IpVideoFeed* getIpVideoFeed();

public slots:
    void setUrl(QString url);
    void enableUrlEditing();
    void disableUrlEditing();

signals:
    void autoGenerateClicked(bool enabled);

private slots:
    void onAutoGenerateClicked();
    
private:
    Ui::IpVideoFeedSettingsWidget *ui;
    IpVideoFeed *m_ipVideoFeed;
};

#endif // IPVIDEOFEEDSETTINGSWIDGET_H
