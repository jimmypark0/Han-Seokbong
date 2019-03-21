package com.kobot.op.hsbforandroid;

import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.StringTokenizer;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Message {
	public static final boolean NOT = false;
	public static final boolean GEN = true;

	private SimpleDateFormat time = new SimpleDateFormat("HH:mm:ss");
	private byte[] genMsg = new byte[1024];
	private byte[] notMsg = new byte[64];

	private Queue<byte[]> q = new LinkedList<byte[]>();
	private Lock lock = new ReentrantLock();

	//methods for message building.
	public void setMode(int mode) { genMsg[0] = (byte)(mode << 1); }

	public void setTime(Date date) {
		StringTokenizer tokTime = new StringTokenizer(time.format(date), ":");

		for(int i = 1; i < 4; i++)
			genMsg[i] = Byte.parseByte(tokTime.nextToken());
	}

	public void buildMessage(boolean isGen, String text) {
		if(isGen) {
			List<String> tTexts = new ArrayList<String>((text.length() + 340 - 1) / 340);

			for (int i = 0; i < text.length(); i += 340)
				tTexts.add(text.substring(i, Math.min(text.length(), i + 340)));

			for (int i = 0; i < tTexts.size(); i++) {
				//type field (1 byte : 0)
				if (i == tTexts.size() - 1) genMsg[0] += 1;

				//text field (1020 bytes : 4 ~ 1024)
				byte[] token = tTexts.get(i).getBytes();
				int len = token.length;

				System.arraycopy(token, 0, genMsg, 4, len);
				for (int j = len; j < 1020; j++) genMsg[j + 4] = 0;

				q.offer(genMsg.clone());
			}
		} else {
			byte[] temp = text.getBytes();
			System.arraycopy(temp, 0, notMsg, 0, temp.length);
			for (int i = temp.length; i < 64; i++) notMsg[i] = 0;
			q.offer(notMsg.clone());
		}

		this.setMode(0);
	}

	public byte[] getMessage() { return q.poll(); }
}