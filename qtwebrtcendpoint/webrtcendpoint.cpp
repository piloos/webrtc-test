#include <webrtcendpoint.h>
#include <QtCore/QDebug>
#include <QtGlobal>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

#include <gst/gst.h>
#include <gst/sdp/gstsdpmessage.h>
#include <gst/gstpad.h>
#include <gst/gstcaps.h>

#include <kurento/commons/kmselementpadtype.h>
#include <kurento/commons/kmsmediatype.h>
#include <kurento/webrtcendpoint/kmsicecandidate.h>

static const gchar *rsa_pem = "-----BEGIN PRIVATE KEY-----\r\n"
                              "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCb+LTrbVIUWgpo\r\n"
                              "2P1xIONodNWBZrjKoSiuDFgmwHvRtbsHu3/wVHBw8aCgMfSAkx5fr/qE3V2u9Ufc\r\n"
                              "OF2Sm2+n6FpSl4n3Y0Pj06GkoZ3G2Q68Pce53jOpud1TJbFT9cPl4zUnz+36fczB\r\n"
                              "US9nNsHHEvkPXu1NbNWsf2/cRB3nSUlENz0lJYMDnNQwmE1IALFvxccY3cCUUsku\r\n"
                              "3hUJiK9SSSHvGOT41xW55IpfjL03HEyU+eBzo+KqCgZ7GPtaYdbMtg6AOIktRIiK\r\n"
                              "NjoMU5Kmv5XY/wdOZFX42pSit3SyBZXIdlJ2/cilRzHdTpX0+FJaQOsE0cmADqUu\r\n"
                              "Z9xQVxYVAgMBAAECggEAeTfDtC0UW4jXCkGKR3/d/XK/9H/XInQ533rsj8GM+aEZ\r\n"
                              "qJrKhge6E61WvunUMpEkTA3Cz7wTcURkAv0zjBuNnZtxhzsLGN9nBguWVxLcQoyz\r\n"
                              "bQ0+ym+tCz3Oiy6CMNSS2XnJ+BUbyVA38A6ensLpu0Q9mPqXx1LMhfHNCA1QiE0w\r\n"
                              "KtmgLaCH55x7fhWJXCkQmsD2Ir8JrS6y3UYQ9fEoFT/qxPyIt77f6cYvsd2zdtWv\r\n"
                              "LvsKG8YeLtbJAgmO0dSDuTKOEIjvjfOio3geVGNXcAa12i7aSsaHElk8OEmTTTDI\r\n"
                              "s8KkqHtuK+/vzj09dvPtT/VFF6NwY+Chjw1aOwGLSQKBgQDKOuPv8zJt3uIiYnoD\r\n"
                              "uSFdReK4daGa/lLrmQ0eEUsRT8hcMV/oZ4nxPh8iNAPVwxtsXGyZ7XTguXrG+Zxq\r\n"
                              "me+9y0A5Nzy+iNMDhQltJP5tnaIVy/GoS2SmPYqz4QcgU//77YZgnKsm3B6vIjJa\r\n"
                              "cs8P0gmiU/1ps/5HwJxZjil33wKBgQDFcSfmotKJLPM2cP/XiYhSqrAy0SrvbvmQ\r\n"
                              "Bbupsr0wnAAfu+/SrbYCUA2x8//Qs8dtZBjTNgQYR+26zWDG2xuHuOwKfG0CYzVk\r\n"
                              "8CHDmlBpM/Jv5H993SXlTj5sp+LA6tplprq0v+9sXINk5R+SP4SaZi+t3zKk0bK2\r\n"
                              "EiymJo0AiwKBgGX5cizx7lD23gLGs44jEU7uSZgIQPheHEQPBk1OHRNarsbGYv1X\r\n"
                              "EHjkgWCG6BQncTGgHWc1FQWood+pyJT8kKac0gLH9sqBRh51PD6cM+vkW/Ivx+i8\r\n"
                              "M8GcMM/pveUwVlmb+XHILonG33YigU+Yqw7oM9F2FsfxalyWJIEILaLzAoGAQWGV\r\n"
                              "OvSUD0TJTS/iKLesYuOO8WT+eMcg8SZU3H8J0zz1dYzAf91yNhXYUyNfhPbjhT/u\r\n"
                              "UJLEjF3VRVSZRYBs/2anE1ncpzu/BKvYInPJSO0gzRi3dzByShq85TI7DnM22w55\r\n"
                              "KT2dxR5ljFWrPLy35oEMLOGKXbXrHguqqWJ+sr0CgYAihBSmBp5KUtR+3ywB4XVF\r\n"
                              "V56MRokU2vrvYO071L5VGfw2aQgj71Mnyqou82RhEpSfO6INsbh++KkcXvgamyK9\r\n"
                              "ecXXDlHpfaQqW9uKDMSrSjIS/stw1vPuaQ5aUYt+zSPkEZtQitDo3mtoYd75gznz\r\n"
                              "oCwhRa+5PS9/8qiujp3WXw==\r\n"
                              "-----END PRIVATE KEY-----\r\n"
                              "-----BEGIN CERTIFICATE-----\r\n"
                              "MIICtzCCAZ+gAwIBAgIBADANBgkqhkiG9w0BAQsFADAfMQswCQYDVQQGEwJTRTEQ\r\n"
                              "MA4GA1UEAwwHS3VyZW50bzAeFw0xNjA2MzAxMDUxMzhaFw0xNzA2MzAxMDUxMzha\r\n"
                              "MB8xCzAJBgNVBAYTAlNFMRAwDgYDVQQDDAdLdXJlbnRvMIIBIjANBgkqhkiG9w0B\r\n"
                              "AQEFAAOCAQ8AMIIBCgKCAQEAm/i0621SFFoKaNj9cSDjaHTVgWa4yqEorgxYJsB7\r\n"
                              "0bW7B7t/8FRwcPGgoDH0gJMeX6/6hN1drvVH3Dhdkptvp+haUpeJ92ND49OhpKGd\r\n"
                              "xtkOvD3Hud4zqbndUyWxU/XD5eM1J8/t+n3MwVEvZzbBxxL5D17tTWzVrH9v3EQd\r\n"
                              "50lJRDc9JSWDA5zUMJhNSACxb8XHGN3AlFLJLt4VCYivUkkh7xjk+NcVueSKX4y9\r\n"
                              "NxxMlPngc6PiqgoGexj7WmHWzLYOgDiJLUSIijY6DFOSpr+V2P8HTmRV+NqUord0\r\n"
                              "sgWVyHZSdv3IpUcx3U6V9PhSWkDrBNHJgA6lLmfcUFcWFQIDAQABMA0GCSqGSIb3\r\n"
                              "DQEBCwUAA4IBAQCNE5tE/ohbaWOTossq2PmYypJjitHUoHfheR9dT5vYm2Izla+z\r\n"
                              "AHZVODp9r/25EG3VjKXshL0rSV3ERC5P0wqGBaKCDRJ4pycfA4Fz93byH4r8/6xL\r\n"
                              "EcRsG6F8vsMht1yTjq1zFCNN+OeWJtQmXCKKFKLY4+lMsnGyJJGXlW1yJe7D9x9Q\r\n"
                              "32DO9KIiRiju+ATHMtrwPwTMgg5Gqd+HmVKhTwcb5RbGwP/xCcK44NLGBdxD8eNi\r\n"
                              "bqedNYytvSmEQGEuwlwtA0fNAetr5x7Qegfl4vTWTKogna1xm7SSYqNeOYJZYauV\r\n"
                              "1E1yY33Cjjz/BFBW6lqcl6ryeqzTwg/GXXGW\r\n" "-----END CERTIFICATE-----";

static void bus_msg(GstBus * bus, GstMessage * msg, gpointer pipe)
{
    Q_UNUSED(bus);
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:{
        qCritical("Error: %" GST_PTR_FORMAT, msg);
        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN (pipe), GST_DEBUG_GRAPH_SHOW_ALL, "error");
        break;
    }
    case GST_MESSAGE_WARNING:{
        qWarning("Warning: %" GST_PTR_FORMAT, msg);
        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (pipe), GST_DEBUG_GRAPH_SHOW_ALL, "warning");
        break;
    }
    default:
        break;
    }
}

static GArray *create_codecs_array (gchar * codecs[])
{
    GArray *a = g_array_new (FALSE, TRUE, sizeof (GValue));
    guint8 i;

    for (i = 0; i < g_strv_length (codecs); i++) {
        GValue v = G_VALUE_INIT;
        GstStructure *s;

        g_value_init (&v, GST_TYPE_STRUCTURE);
        s = gst_structure_new (codecs[i], NULL, NULL);
        gst_value_set_structure (&v, s);
        gst_structure_free (s);
        g_array_append_val (a, v);
    }

    return a;
}

static void cb_ice_candidate(GstElement *element, gchararray arg0, KmsIceCandidate *arg1, gpointer user_data)
{
    Q_UNUSED(element);
    Q_UNUSED(arg0);

    QJsonObject inner;
    inner.insert("candidate", QJsonValue(kms_ice_candidate_get_candidate(arg1)));
    inner.insert("sdpMid", QJsonValue(kms_ice_candidate_get_sdp_mid(arg1)));
    inner.insert("sdpMLineIndex", QJsonValue(kms_ice_candidate_get_sdp_m_line_index(arg1)));
    QJsonObject outer;
    outer.insert("icie", QJsonValue(inner));
    QJsonDocument doc(outer);
    QString ice_candidate_json(doc.toJson(QJsonDocument::Compact));
    WebRTCEndpoint *webrtcendpoint = (WebRTCEndpoint*) user_data;
    qDebug("Callback, ICE candidate generated");
    qDebug() << ice_candidate_json;
    emit webrtcendpoint->sendMessage(ice_candidate_json);
}

static void cb_new_pad(GstElement *element, GstPad *pad, gpointer user_data)
{
    Q_UNUSED(element);

    qDebug("Callback, new pad!!");
    gchar* padname = gst_pad_get_name(pad);
    qDebug() << QString("padname: %1").arg(padname);

    WebRTCEndpoint *webrtcendpoint = (WebRTCEndpoint*) user_data;

    GstCaps *caps = gst_pad_get_pad_template_caps(pad);
    gchar *caps_str = gst_caps_to_string(caps);
    qDebug() << QString("caps: %1").arg(caps_str);
    GstPadDirection pad_direction = gst_pad_get_direction(pad);
    const char *dir_str = (pad_direction == GST_PAD_SRC) ? "source pad" : "sink pad";
    qDebug() << QString("direction: %1").arg(dir_str);
    g_free(caps_str);
    gst_caps_unref(caps);

    if(pad_direction == GST_PAD_SRC)
    {
        //it is a source pad, meaning we need to retrieve data from it
        GstPad *sinkpad = gst_element_get_static_pad(webrtcendpoint->decoder, "sink");
        gst_pad_link(pad, sinkpad);
        gst_object_unref(sinkpad);

        if(!gst_element_sync_state_with_parent(webrtcendpoint->decoder))
            qCritical("ERROR: could not sync state of decoder with parent");

        if(!gst_element_sync_state_with_parent(webrtcendpoint->display))
            qCritical("ERROR: could not sync state of display with parent");
    }
    else if(pad_direction == GST_PAD_SINK)
    {
        //it is a sink pad, meaning we need to push data to it
        GstPad* sourcepad = gst_element_get_static_pad(webrtcendpoint->encoder, "src");
        gst_pad_link(sourcepad, pad);
        gst_object_unref(sourcepad);

        if(!gst_element_sync_state_with_parent(webrtcendpoint->input))
            qCritical("ERROR: could not sync state of input with parent");

        if(!gst_element_sync_state_with_parent(webrtcendpoint->encoder))
            qCritical("ERROR: could not sync state of encoder with parent");
    }
    else {
        qCritical() << "Pad without direction!";
    }

    g_free(padname);
}

WebRTCEndpoint::WebRTCEndpoint(QObject *parent) : QObject(parent)
{
    //internal housekeeping
    QObject::connect(this, SIGNAL(incoming_SDP_answer(QString)), this, SLOT(on_SDP_answer(QString)));
    QObject::connect(this, SIGNAL(incoming_SDP_offer(QString)), this, SLOT(on_SDP_offer(QString)));
    QObject::connect(this, SIGNAL(incoming_ICE_candidate(KmsIceCandidate*)), this, SLOT(on_ICE_candidate(KmsIceCandidate*)));

    //create the pipeline
    pipeline = gst_pipeline_new(NULL);
    display = gst_element_factory_make("autovideosink", NULL);
    decoder = gst_element_factory_make("avdec_h264", NULL);

    input = gst_element_factory_make("videotestsrc", NULL);
    encoder = gst_element_factory_make("x264enc", NULL);

    webrtc_receiver = gst_element_factory_make("webrtcendpoint", NULL);
    if(!pipeline || !webrtc_receiver || !display || !decoder || !input || !encoder) {
        qCritical("ERROR: could not create all elements!");
    }

    //catch bus messages
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
    gst_bus_add_watch(bus, gst_bus_async_signal_func, NULL);
    g_signal_connect(bus, "message", G_CALLBACK (bus_msg), pipeline);
    g_object_unref(bus);

    //start the pipeline
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if(ret != GST_STATE_CHANGE_SUCCESS) {
        qCritical() << QString("Error: Pipeline did not get started (return: %1)!").arg(ret);
    }

    //blocking wait until pipeline is running
    GstState state;
    ret = gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
    qDebug() << QString("Pipeline got started (state: %1)").arg(state);

    //add elements to pipeline and sync
    gst_bin_add(GST_BIN(pipeline), display);
    gst_bin_add(GST_BIN(pipeline), decoder);
    gst_bin_add(GST_BIN(pipeline), input);
    gst_bin_add(GST_BIN(pipeline), encoder);
    gst_bin_add(GST_BIN(pipeline), webrtc_receiver);

    if(!gst_element_sync_state_with_parent(webrtc_receiver))
        qCritical("ERROR: could not sync state of webrtc_receiver with parent");

    //link sink chain
    gst_element_link(decoder, display);

    //link source chain
    gst_element_link(input, encoder);

    //connect the callbacks
    gint id1 = g_signal_connect(webrtc_receiver, "pad-added", G_CALLBACK (cb_new_pad), this);
    qDebug() << QString("Handler pad-added id: %1").arg(id1);
    gint id2 = g_signal_connect(webrtc_receiver, "on-ice-candidate", G_CALLBACK(cb_ice_candidate), this);
    qDebug() << QString("Handler on-ice-candidate id: %1").arg(id2);

    //set the video/audio sesttings
    gchar *codecs[] = { (gchar*) "H264/90000", NULL };
    GArray *codecs_array = create_codecs_array(codecs);
    g_object_set(webrtc_receiver, "num-video-medias", 1 , "video-codecs", g_array_ref(codecs_array), "bundle", 1, NULL);
    g_array_unref(codecs_array);

    //set PEM certificate
    g_object_set (webrtc_receiver, "pem-certificate", rsa_pem, NULL);

    //set the STUN server
    g_object_set (webrtc_receiver, "stun-server", "74.125.143.127", NULL);
    g_object_set (webrtc_receiver, "stun-server-port", 19302, NULL);

    //create the session
    g_signal_emit_by_name (webrtc_receiver, "create-session", &receiver_sess_id);
    qDebug() << QString("Created receiver session with id %1").arg(receiver_sess_id);

    //request source pad
    gchar *padname;
    g_signal_emit_by_name (webrtc_receiver, "request-new-pad", KMS_ELEMENT_PAD_TYPE_VIDEO, NULL, GST_PAD_SRC, &padname);
    qDebug() << QString("Requested src pad: %1").arg(padname);

}

WebRTCEndpoint::~WebRTCEndpoint() {}

void WebRTCEndpoint::dump_pipeline()
{
    GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
    qDebug() << "pipeline dump generated!";
}

void WebRTCEndpoint::start_session()
{
    QString offer;
    create_SDP_offer(offer);

    QJsonObject inner;
    inner.insert("type", QJsonValue("offer"));
    inner.insert("sdp", QJsonValue(offer));
    QJsonObject outer;
    outer.insert("sdp", QJsonValue(inner));
    QJsonDocument doc(outer);
    QString offer_json(doc.toJson(QJsonDocument::Compact));
    qDebug() << offer_json;
    emit sendMessage(offer_json);
}

bool WebRTCEndpoint::create_SDP_offer(QString &offer)
{
    GstSDPMessage *generated_offer;
    g_signal_emit_by_name(webrtc_receiver, "generate-offer", receiver_sess_id, &generated_offer);
    gchar *sdp_str = gst_sdp_message_as_text(generated_offer);
    qDebug() << QString("Generated offer: %1").arg(sdp_str);
    offer = QString(sdp_str);
    return true;
}

bool WebRTCEndpoint::process_SDP_offer(QString offer, QString &answer)
{
    GstSDPMessage *received_offer;
    GstSDPResult sdp_result = gst_sdp_message_new(&received_offer);
    if(sdp_result != GST_SDP_OK) {
        qCritical("Error, could not initialize SDP message!");
        return false;
    }

    sdp_result = gst_sdp_message_parse_buffer((const guint8*) offer.toStdString().c_str(), -1, received_offer);
    if(sdp_result != GST_SDP_OK) {
        qCritical("Error, could not parse SDP offer!");
        return false;
    }
    else {
        qDebug("Successfully parsed SDP offer into SDP message");
    }
    GstSDPMessage *generated_answer;
    g_signal_emit_by_name(webrtc_receiver, "process-offer", receiver_sess_id, received_offer, &generated_answer);
    gchar *sdp_str = gst_sdp_message_as_text(generated_answer);
    qDebug() << QString("Generated answer: %1").arg(sdp_str);
    answer = QString(sdp_str);
    return true;
}

bool WebRTCEndpoint::process_SDP_answer(QString answer)
{
    GstSDPMessage *received_answer;
    GstSDPResult sdp_result = gst_sdp_message_new(&received_answer);
    if(sdp_result != GST_SDP_OK) {
        qCritical("Error, could not create SDP message!");
        return false;
    }

    sdp_result = gst_sdp_message_parse_buffer((const guint8*) answer.toStdString().c_str(), -1, received_answer);
    if(sdp_result != GST_SDP_OK) {
        qCritical("Error, could not parse SDP answer!");
        return false;
    }
    else {
        qDebug("Successfully parsed SDP answer into SDP message");
    }
    gboolean gret;
    g_signal_emit_by_name (webrtc_receiver, "process-answer", receiver_sess_id, received_answer, &gret);
    if(gret != TRUE) {
        qCritical("Error: could not process answer!");
        return true;
    }
    else {
        qDebug("Successfully processed answer!");
        return true;
    }
}

bool WebRTCEndpoint::gather_ICE_candidates()
{
    gboolean gret;
    g_signal_emit_by_name (webrtc_receiver, "gather-candidates", receiver_sess_id, &gret);
    if(gret != TRUE) {
        qCritical("Error: could not gather ICE candidates!");
        return false;
    }
    else {
        qDebug("Successfully started gathering ICE candidates!");
        return true;
    }
}

void WebRTCEndpoint::onMessageReceived(QString message)
{
    qDebug() << "WebRTCEndpoint: received messsage:" << message;

    QJsonObject msg_json = QJsonDocument::fromJson(message.toUtf8()).object();

    QStringList keys = msg_json.keys();
    if(keys.length() != 1) {
        qCritical() << QString("Not expecting JSON mesages with %1 keys.  Expecting exactly 1.").arg(keys.length());
        return;
    }

    QString message_type = keys.first();
    if(message_type == "sdp") {
        QJsonObject sdp_json = msg_json[message_type].toObject();
        QStringList keys = sdp_json.keys();
        if(keys.length() != 2) {
            qCritical() << QString("Not expecting SDP JSON mesages with %1 keys.  Expecting exactly 2.").arg(keys.length());
            return;
        }

        QString sdp_type = sdp_json["type"].toString();
        if(sdp_type == "offer") {
            QString offer = sdp_json["sdp"].toString();
            emit incoming_SDP_offer(offer);
        }
        else if(sdp_type == "answer") {
            QString answer = sdp_json["sdp"].toString();
            emit incoming_SDP_answer(answer);
        }
    }
    else if(message_type == "icie") {
        QJsonObject inner = msg_json[message_type].toObject();
        QString candidate = inner["candidate"].toString();
        QString sdpMid = inner["sdpMid"].toString();
        guint8 sdpMLineIndex = inner["sdpMLineIndex"].toInt();
        KmsIceCandidate* kms = kms_ice_candidate_new((const gchar*) candidate.toStdString().c_str(), (const gchar*) sdpMid.toStdString().c_str(), sdpMLineIndex, "8");
        emit incoming_ICE_candidate(kms);
    }
    else {
        qCritical() << QString("Unknown message type: %1").arg(message_type);
    }
}

void WebRTCEndpoint::on_SDP_answer(QString answer)
{
    bool ret = process_SDP_answer(answer);
    if(ret)
        qDebug() << "Successfully processed SDP answer";
    else
        qCritical() << "Could not process SDP answer";

    this->gather_ICE_candidates();
}

void WebRTCEndpoint::on_SDP_offer(QString offer)
{
    QString answer;
    bool ret = process_SDP_offer(offer, answer);

    if(ret)
        qDebug() << "Successfully processed SDP offer";
    else
        qCritical() << "Could not process SDP offer";

    QJsonObject inner;
    inner.insert("type", QJsonValue("answer"));
    inner.insert("sdp", QJsonValue(answer));
    QJsonObject outer;
    outer.insert("sdp", QJsonValue(inner));
    QJsonDocument doc(outer);
    QString answer_json(doc.toJson(QJsonDocument::Compact));
    qDebug() << answer_json;
    emit sendMessage(answer_json);

    this->gather_ICE_candidates();
}

void WebRTCEndpoint::on_ICE_candidate(KmsIceCandidate* ICE_candidate)
{
    gboolean gret;
    qDebug() << "Processing" << kms_ice_candidate_get_candidate(ICE_candidate)
             << kms_ice_candidate_get_sdp_mid(ICE_candidate)
             << kms_ice_candidate_get_sdp_m_line_index(ICE_candidate);
    g_signal_emit_by_name(webrtc_receiver, "add-ice-candidate", receiver_sess_id, ICE_candidate, &gret);
    if(gret != TRUE) {
        qCritical("Error: could not add ICE candidate!");
    }
    else {
        qDebug("Successfully added ICE candidate!");
    }
}
