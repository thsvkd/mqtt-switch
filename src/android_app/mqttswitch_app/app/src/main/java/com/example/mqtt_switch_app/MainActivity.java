package com.example.mqtt_switch_app;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.view.View;

import android.widget.Button;
import android.widget.TextView;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import android.util.Log;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "MainActivity";

    Button button1;
    Button button2;
    TextView TextView1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        button1 = (Button)findViewById(R.id.button1);
        button2 = (Button)findViewById(R.id.button2);
        TextView1 = (TextView)findViewById(R.id.textView1);

        this.SetListener();
    }

    public void mqtt_pub(String topic, String content, int qos, String broker, String clientId)
    {
        MemoryPersistence persistence = new MemoryPersistence();

        Log.d(TAG, "setting Broker\n");

        try {
            MqttClient sampleClient = new MqttClient(broker, clientId, persistence);
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(true);
            System.out.println("Connecting to broker: "+broker);
            sampleClient.connect(connOpts);
            System.out.println("Connected");
            System.out.println("Publishing message: "+content);
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            sampleClient.publish(topic, message);
            System.out.println("Message published");
//            sampleClient.disconnect();
//            System.out.println("Disconnected");
//            System.exit(0);
        } catch(MqttException me) {
            System.out.println("reason "+me.getReasonCode());
            System.out.println("msg "+me.getMessage());
            System.out.println("loc "+me.getLocalizedMessage());
            System.out.println("cause "+me.getCause());
            System.out.println("excep "+me);
            me.printStackTrace();
        }
    }

    public void onClick(View view)
    {
        if (view.getId() == R.id.button1)
        {
            mqtt_pub("thsvkd/switch", "0", 1, "tcp://broker.emqx.io", "thsvkd");
        } else if (view.getId() == R.id.button2)
        {
            mqtt_pub("thsvkd/switch", "1", 1, "tcp://broker.emqx.io", "thsvkd");
        }
    }

    public void SetListener()
    {
        button1.setOnClickListener(this);
        button2.setOnClickListener(this);
    }
}