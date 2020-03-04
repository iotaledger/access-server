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

package com.de.xain.emdac;

import android.arch.lifecycle.ViewModel;
import android.databinding.Bindable;
import android.databinding.Observable;
import android.databinding.PropertyChangeRegistry;
import android.support.annotation.NonNull;

public class BaseObservableViewModel extends ViewModel implements Observable {
        private transient PropertyChangeRegistry mCallbacks;

        public BaseObservableViewModel() {
        }

        @Override
        public void addOnPropertyChangedCallback(@NonNull Observable.OnPropertyChangedCallback callback) {
            synchronized (this) {
                if (mCallbacks == null) {
                    mCallbacks = new PropertyChangeRegistry();
                }
            }
            mCallbacks.add(callback);
        }

        @Override
        public void removeOnPropertyChangedCallback(@NonNull Observable.OnPropertyChangedCallback callback) {
            synchronized (this) {
                if (mCallbacks == null) {
                    return;
                }
            }
            mCallbacks.remove(callback);
        }

        /**
         * Notifies listeners that all properties of this instance have changed.
         */
        public void notifyChange() {
            synchronized (this) {
                if (mCallbacks == null) {
                    return;
                }
            }
            mCallbacks.notifyCallbacks(this, 0, null);
        }

        /**
         * Notifies listeners that a specific property has changed. The getter for the property
         * that changes should be marked with {@link Bindable} to generate a field in
         * <code>BR</code> to be used as <code>fieldId</code>.
         *
         * @param fieldId The generated BR id for the Bindable field.
         */
        public void notifyPropertyChanged(int fieldId) {
            synchronized (this) {
                if (mCallbacks == null) {
                    return;
                }
            }
            mCallbacks.notifyCallbacks(this, fieldId, null);
        }
}
