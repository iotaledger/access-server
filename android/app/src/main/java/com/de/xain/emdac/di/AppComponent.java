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
