#ifndef WEBRTCENDPOINT_H
#define WEBRTCENDPOINT_H

#include <QtCore/QObject>
#include <gst/gst.h>

#include <kurento/webrtcendpoint/kmsicecandidate.h>

class WebRTCEndpoint : public QObject
{
    Q_OBJECT

public:
    explicit WebRTCEndpoint(QObject *parent = Q_NULLPTR);
    virtual ~WebRTCEndpoint();

    GstElement *display;
    GstElement *decoder;
    GstElement *input;
    GstElement *encoder;

signals:
    void sendMessage(QString message);
    void incoming_SDP_offer(QString offer);
    void incoming_SDP_answer(QString offer);
    void incoming_ICE_candidate(KmsIceCandidate* ICE_candicate);

public slots:
    void onMessageReceived(QString message);
    void start_session();
    void dump_pipeline(void);

private slots:
    void on_SDP_offer(QString offer);
    void on_SDP_answer(QString offer);
    void on_ICE_candidate(KmsIceCandidate* ICE_candidate);

private:
    bool create_SDP_offer(QString &offer);
    bool process_SDP_offer(QString offer, QString &answer);
    bool process_SDP_answer(QString answer);
    bool gather_ICE_candidates();

private:
    GstElement *pipeline;
    GstElement *webrtc_receiver;
    gchar *receiver_sess_id;
};

#endif // WEBRTCENDPOINT_H

