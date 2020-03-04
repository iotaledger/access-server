/*
 *  This file is part of the Frost distribution
 *  (https://github.com/xainag/frost)
 *
 *  Copyright (c) 2019 XAIN AG.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package com.de.xain.emdac.api.asr;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import android.support.annotation.MainThread;

import java.util.List;

import javax.inject.Inject;
import javax.inject.Singleton;

import timber.log.Timber;

/**
 * Class for using Automatic Speech Recognition (ASR).
 */
@Singleton
public class ASRClient implements RecognitionListener {

    private SpeechRecognizer mSpeechRecognizer;
    private Context mContext;
    private String mLanguage;

    @Inject
    public ASRClient(Context context) {
        this.mContext = context;
        this.mLanguage = "en-US";
    }

    /**
     * Method that starts speech recognition.
     */
    @MainThread
    public void startListening() throws ASRClientException {
        PackageManager packageManager = mContext.getPackageManager();

        List intActivities = packageManager
                .queryIntentActivities(new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH), 0);

        if (intActivities.size() == 0) {
            throw new ASRClientException("SpeechRecognizer not supported");
        }

        mSpeechRecognizer = SpeechRecognizer.createSpeechRecognizer(mContext);
        mSpeechRecognizer.setRecognitionListener(this);

        Intent intent = generateIntent();

        mSpeechRecognizer.startListening(intent);

        Timber.d("Did start ASR listening.");
    }

    public Intent generateIntent() {
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

        // The extra key used in an intent to the speech recognizer for voice search.
        intent.putExtra(RecognizerIntent.EXTRA_CALLING_PACKAGE, mContext.getPackageName());

        // Optional limit on the maximum number of results to return.
        intent.putExtra(RecognizerIntent.EXTRA_MAX_RESULTS, 1);

        // Optional IETF language tag (as defined by BCP 47), for example "en-US".
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, mLanguage);

        // Informs the recognizer which speech model to prefer when
        // performing ACTION_RECOGNIZE_SPEECH.
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);

        // Optional boolean to indicate whether partial results should be returned by
        // the recognizer as the user speaks (default is false).
        intent.putExtra(RecognizerIntent.EXTRA_PARTIAL_RESULTS, true);

        // The minimum length of an utterance.
        intent.putExtra(RecognizerIntent.
                EXTRA_SPEECH_INPUT_MINIMUM_LENGTH_MILLIS, 3000);

        // The amount of time that it should take after we stop hearing
        // speech to consider the input complete.
        intent.putExtra(RecognizerIntent.
                EXTRA_SPEECH_INPUT_COMPLETE_SILENCE_LENGTH_MILLIS, 4000);

        // The amount of time that it should take after we stop hearing
        // speech to consider the input possibly complete.
        intent.putExtra(RecognizerIntent.
                EXTRA_SPEECH_INPUT_POSSIBLY_COMPLETE_SILENCE_LENGTH_MILLIS, 3000);

        return intent;
    }

    /**
     * Method that stops speech recognition.
     */
    public void stopListening() {
        if (mSpeechRecognizer != null) {
            mSpeechRecognizer.destroy();
        }
        Timber.d("Did stop ASR listening.");
    }

    @Override
    public void onReadyForSpeech(Bundle params) {

    }

    @Override
    public void onBeginningOfSpeech() {

    }

    @Override
    public void onRmsChanged(float rmsdB) {

    }

    @Override
    public void onBufferReceived(byte[] buffer) {

    }

    @Override
    public void onEndOfSpeech() {

    }

    @Override
    public void onError(int error) {

    }

    @Override
    public void onResults(Bundle results) {

    }

    @Override
    public void onPartialResults(Bundle partialResults) {

    }

    @Override
    public void onEvent(int eventType, Bundle params) {

    }
}
