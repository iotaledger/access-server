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

package com.de.xain.emdac.di;

import android.app.Application;

import com.de.xain.emdac.EmdacApp;
import com.de.xain.emdac.di.module.ActivityModule;
import com.de.xain.emdac.di.module.AppModule;
import com.de.xain.emdac.di.module.ContextModule;
import com.de.xain.emdac.di.module.InterfaceModule;
import com.de.xain.emdac.di.module.NativeLibModule;
import com.de.xain.emdac.di.module.SharedPreferencesModule;
import com.de.xain.emdac.di.module.ThemeLabModule;
import com.de.xain.emdac.user.UserModule;

import javax.inject.Singleton;

import dagger.BindsInstance;
import dagger.Component;
import dagger.android.AndroidInjectionModule;
import dagger.android.support.AndroidSupportInjectionModule;

/**
 * Created by flaviu.lupu on 14-Nov-17.
 * Application component providing different modules
 */

@Singleton
@Component(modules = {
        AndroidInjectionModule.class,
        ContextModule.class,
        SharedPreferencesModule.class,
        AppModule.class,
        ActivityModule.class,
        InterfaceModule.class,
        NativeLibModule.class,
        UserModule.class,
        ThemeLabModule.class,
        AndroidSupportInjectionModule.class
})
public interface AppComponent {
    @Component.Builder
    interface Builder {
        @BindsInstance
        Builder application(Application application);

        Builder contextModule(ContextModule contextModule);

        AppComponent build();
    }

    void inject(EmdacApp emdacApp);
}
