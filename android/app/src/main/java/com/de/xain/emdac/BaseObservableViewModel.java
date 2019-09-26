/*
 *  This file is part of the DAC distribution (https://github.com/xainag/frost)
 *  Copyright (c) 2019 XAIN AG.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
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
