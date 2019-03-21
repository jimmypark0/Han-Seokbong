package com.kobot.op.hsbforandroid;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

public class HSB extends Activity implements RecognitionListener {
	//__VARIABLES___________________________________________________________________________________
	private Context context = this;

	//constants
	private static enum V2TS { ERROR, NONE, STARTER, START, END }

	//android widgets : activity_hsb
	private Button endBtn, itemBtn, concBtn, status;

	//android widgets : login_dialog
	private AlertDialog login;
	private EditText editName;
	private TextView help;

	//android widgets : info_dialog
	private AlertDialog mInfo;
	private EditText editMName;

	//speech to text
	private SpeechRecognizer speech;
	private Intent intent;
	private Timer speechTimeout;
	private AudioManager audio;
	public boolean isEnd = true;

	//message
	private Message msg = new Message();
	private String uName;
	private SimpleDateFormat time = new SimpleDateFormat("yyyy.MM.dd HH:mm:ss");

	//networking
	private Socket socket;
	private SendManager sMan = new SendManager();
	private RecvManager rMan = new RecvManager();

	//__ACTIVITY SETTINGS___________________________________________________________________________
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_hsb);

		//android widgets
		endBtn = (Button) findViewById(R.id.endBtn);
		itemBtn = (Button) findViewById(R.id.itemBtn);
		concBtn = (Button) findViewById(R.id.concBtn);
		status = (Button) findViewById(R.id.status);

		//speech to text
		speech = SpeechRecognizer.createSpeechRecognizer(this);
		speech.setRecognitionListener(this);

		intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
		intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
		intent.putExtra(RecognizerIntent.EXTRA_CALLING_PACKAGE, this.getPackageName());

		audio = (AudioManager) getSystemService(Context.AUDIO_SERVICE);

		//Listeners
		endBtn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				String end = "_end" + time.format(new Date());
				msg.buildMessage(Message.NOT, end);
				isEnd = true;
				speech.stopListening();

				mInfo.show();
			}
		});

		itemBtn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				msg.setMode(1);
				loopHandler();

				itemBtn.setBackgroundColor(0xFF6799FF);
			}
		});

		concBtn.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				msg.setMode(2);
				loopHandler();

				concBtn.setBackgroundColor(0xFF6799FF);
			}
		});

		status.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				if((isEnd = !isEnd)) {
					status.setBackgroundResource(R.drawable.mute);
					speech.stopListening();
					return;
				}

				loopHandler();
			}
		});

		//setting for login dialog
		login = createLIDialog();
		View liLayout = ((LayoutInflater) getApplicationContext()
				.getSystemService(LAYOUT_INFLATER_SERVICE))
				.inflate(R.layout.login_dialog, (ViewGroup)findViewById(R.id.unameLayout));
		login.setView(liLayout);

		help = (TextView) liLayout.findViewById(R.id.help);
		editName = (EditText) liLayout.findViewById(R.id.uname);
		editName.addTextChangedListener(new TextWatcher() {
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
			public void onTextChanged(CharSequence s, int start, int before, int count) {}
			public void afterTextChanged(Editable editable) {
				if(editable != null && editable.length() != 0)
					login.getButton(AlertDialog.BUTTON_NEUTRAL).setEnabled(true);
			}
		});

		login.show();
		login.getButton(AlertDialog.BUTTON_NEUTRAL).setEnabled(false);
		login.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				uName = editName.getText().toString();

				sMan.start();
				msg.buildMessage(Message.NOT, uName.replace(" ", "_"));

				login.getButton(AlertDialog.BUTTON_NEUTRAL).setEnabled(false);
				login.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(true);

				help.setText("모든 참가자가 접속하였다면 '회의 시작'을 누르세요.");
			}
		});

		login.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
		login.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				String start = "_start" + time.format(new Date());
				msg.buildMessage(Message.NOT, start);
			}
		});

		//setting for meeting info dialog
		mInfo = createMIDialog();
		View miLayout = ((LayoutInflater) getApplicationContext()
				.getSystemService(LAYOUT_INFLATER_SERVICE))
				.inflate(R.layout.info_dialog, (ViewGroup)findViewById(R.id.infoLayout));
		mInfo.setView(miLayout);

		editMName = (EditText) miLayout.findViewById(R.id.mname);
		editMName.addTextChangedListener(new TextWatcher() {
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
			public void onTextChanged(CharSequence s, int start, int before, int count) {}
			public void afterTextChanged(Editable editable) {
				if(editable != null && editable.length() != 0)
					mInfo.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(true);
			}
		});

		mInfo.show();
		mInfo.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
		mInfo.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				String mname = "_mname\n" + editMName.getText();
				msg.buildMessage(Message.NOT, mname);
			}
		});

		mInfo.dismiss();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		speech.destroy();
	}

	private AlertDialog createLIDialog() {
		AlertDialog dialogBox = new AlertDialog.Builder(this)
				.setTitle("사용자 이름").setMessage("이름을 입력하세요.")
				.setPositiveButton("회의 시작", null)
				.setNeutralButton("접속", null)
				.create();
		dialogBox.setCancelable(false);
		return dialogBox;
	}

	private AlertDialog createMIDialog() {
		AlertDialog dialogBox = new AlertDialog.Builder(this)
				.setTitle("회의 정보").setMessage("회의 명을 입력해주세요.")
				.setPositiveButton("회의 등록", null)
				.create();
		dialogBox.setCancelable(false);
		return dialogBox;
	}

	//__RECOGNITION LISTENER________________________________________________________________________
	public void onReadyForSpeech(android.os.Bundle bundle) {
		speechTimeout = new Timer();
		speechTimeout.schedule(new TimeOutHandler(), 5000);
	}
	public void onBeginningOfSpeech() {
		speechTimeout.cancel();

		msg.setTime(new Date());
		status.setBackgroundResource(R.drawable.listening);
	}
	public void onRmsChanged(float v) {}
	public void onBufferReceived(byte[] bytes) {}
	public void onEndOfSpeech() {}
	public void onError(int i) {
		speechTimeout.cancel();
		loopHandler();
	}
	public void onResults(Bundle results) {
		ArrayList<String> matches = results.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
		msg.buildMessage(Message.GEN, matches.get(0));

		itemBtn.setBackgroundColor(0xFFD5D5D5);
		concBtn.setBackgroundColor(0xFFD5D5D5);

		loopHandler();
	}
	public void onPartialResults(android.os.Bundle bundle) {}
	public void onEvent(int i, android.os.Bundle bundle) {}

	//__V2T SERVICE HANDLER_________________________________________________________________________
	public void loopHandler() {
		if(isEnd) return;

		speech.cancel();
		status.setBackgroundResource(R.drawable.waiting);
		speech.startListening(intent);
	}

	public class TimeOutHandler extends TimerTask {
		@Override
		public void run() {
			android.os.Message hMsg = new android.os.Message();
			hMsg.obj = V2TS.ERROR;

			StateHandler.sendMessage(hMsg);
		}
	}

	Handler StateHandler = new Handler() {
		public void handleMessage(android.os.Message m) {
			switch((V2TS) m.obj) {
				case START :
					endBtn.setVisibility(View.GONE);
				case STARTER :
					audio.setStreamMute(AudioManager.STREAM_SYSTEM, true);

					isEnd = false;
					login.dismiss();
					speech.startListening(intent);
					break;
				case END :
					if(socket != null && !socket.isClosed())
						try { socket.close(); }
						catch(IOException e) { Log.d("error", e.toString()); }
						finally {
							isEnd = true;
							speech.stopListening();

							audio.setStreamMute(AudioManager.STREAM_SYSTEM, false);
							Toast.makeText(context, "회의가 종료되었습니다.", Toast.LENGTH_LONG).show();

							mInfo.dismiss();
							//removeMessages(1); //TODO
							finish();
						}
					break;
				case ERROR :
					onError(SpeechRecognizer.ERROR_SPEECH_TIMEOUT);
					break;
				default :
			}
		}
	};

	//__NETWORK MANAGERS____________________________________________________________________________
	public class SendManager extends Thread {
		public void run() {
			DataOutputStream out = null;
			byte[] sendBuff;

			try {
				socket = new Socket("203.246.112.252", 44444);
				//TODO ERASE BELOW
				//socket = new Socket("169.254.24.234", 44444);
				out = new DataOutputStream(socket.getOutputStream());

				//socket.setReuseAddress(true);

				rMan.start();

				while (rMan.isAlive()) {
					if((sendBuff = msg.getMessage()) != null) {
						out.writeInt(sendBuff.length);
						out.write(sendBuff);
					}
				}
			}
			catch(IOException e) { Log.d("error", e.toString()); }
			finally {
				if(out != null)
					try { out.close(); }
					catch(IOException e) { Log.d("error", e.toString()); }

				if(socket != null)
					try { socket.close(); }
					catch(IOException e) { Log.d("error", e.toString()); }
			}
		}
	}

	public class RecvManager extends Thread {
		public void run() {
			android.os.Message hMsg = new android.os.Message();
			hMsg.obj = V2TS.NONE;

			InputStream in = null;
			ByteArrayOutputStream noti = null;
			byte[] receiveBuff = new byte[64];
			int bytesRead;

			try {
				in = socket.getInputStream();
				noti = new ByteArrayOutputStream(1024);

				while(hMsg.obj != V2TS.END) {
					if((bytesRead = in.read(receiveBuff)) != -1) {
						noti.write(receiveBuff, 0, bytesRead);

						if(noti.toString("UTF-8").equals("_startx")) 	hMsg.obj = V2TS.STARTER;
						else if(noti.toString("UTF-8").equals("_start")) 	hMsg.obj = V2TS.START;
						else if(noti.toString("UTF-8").equals("_end")) 		hMsg.obj = V2TS.END;

						StateHandler.sendMessage(hMsg);
						noti.reset();

						hMsg = android.os.Message.obtain();
					}
				}
			}
			catch(IOException e) { Log.d("error", e.toString()); }
			finally {
				if(in != null)
					try { in.close(); }
					catch(IOException e) { Log.d("error", e.toString()); }
				if(noti != null)
					try { noti.close(); }
					catch(IOException e) { Log.d("error", e.toString()); }
			}
		}
	}
}