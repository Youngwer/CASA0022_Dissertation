// utils/mobile-utils.js - 移动端交互优化工具

/**
 * 检测设备类型和特性
 */
export const deviceDetection = {
  // 检测是否为移动设备
  isMobile: () => {
    return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
  },

  // 检测是否为触摸设备
  isTouchDevice: () => {
    return 'ontouchstart' in window || navigator.maxTouchPoints > 0;
  },

  // 检测屏幕尺寸
  getScreenSize: () => {
    const width = window.innerWidth;
    if (width <= 375) return 'xs';
    if (width <= 480) return 'sm';
    if (width <= 768) return 'md';
    if (width <= 1024) return 'lg';
    return 'xl';
  },

  // 检测是否为横屏
  isLandscape: () => {
    return window.innerWidth > window.innerHeight;
  },

  // 检测是否为iOS设备
  isIOS: () => {
    return /iPad|iPhone|iPod/.test(navigator.userAgent);
  },

  // 检测是否为Safari浏览器
  isSafari: () => {
    return /^((?!chrome|android).)*safari/i.test(navigator.userAgent);
  }
};

/**
 * 移动端工具提示管理
 */
export const mobileTooltips = {
  activeTooltip: null,
  hideTimeout: null,

  // 显示工具提示
  show: (element, text) => {
    // 隐藏之前的提示
    mobileTooltips.hide();
    
    // 创建新的提示元素
    const tooltip = document.createElement('div');
    tooltip.className = 'tooltip mobile-visible';
    tooltip.textContent = text;
    document.body.appendChild(tooltip);
    
    mobileTooltips.activeTooltip = tooltip;
    
    // 3秒后自动隐藏
    mobileTooltips.hideTimeout = setTimeout(() => {
      mobileTooltips.hide();
    }, 3000);
  },

  // 隐藏工具提示
  hide: () => {
    if (mobileTooltips.activeTooltip) {
      mobileTooltips.activeTooltip.remove();
      mobileTooltips.activeTooltip = null;
    }
    if (mobileTooltips.hideTimeout) {
      clearTimeout(mobileTooltips.hideTimeout);
      mobileTooltips.hideTimeout = null;
    }
  },

  // 初始化工具提示事件
  init: () => {
    if (!deviceDetection.isTouchDevice()) return;

    // 为所有info图标添加点击事件
    document.addEventListener('click', (e) => {
      if (e.target.matches('.info-icon')) {
        e.preventDefault();
        e.stopPropagation();
        
        const container = e.target.closest('.info-container');
        const tooltip = container?.querySelector('.tooltip');
        
        if (tooltip) {
          mobileTooltips.show(e.target, tooltip.textContent);
        }
      } else {
        // 点击其他地方隐藏提示
        mobileTooltips.hide();
      }
    });
  }
};

/**
 * 视口管理
 */
export const viewportManager = {
  // 设置视口元标签
  setViewport: () => {
    let viewport = document.querySelector('meta[name="viewport"]');
    if (!viewport) {
      viewport = document.createElement('meta');
      viewport.name = 'viewport';
      document.head.appendChild(viewport);
    }
    
    const content = deviceDetection.isMobile() 
      ? 'width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'
      : 'width=device-width, initial-scale=1.0';
      
    viewport.content = content;
  },

  // 监听方向变化
  handleOrientationChange: () => {
    const handleChange = () => {
      // 延迟执行以确保新的尺寸已生效
      setTimeout(() => {
        // 触发重新布局
        window.dispatchEvent(new Event('resize'));
        
        // 更新CSS自定义属性
        document.documentElement.style.setProperty(
          '--vh', 
          `${window.innerHeight * 0.01}px`
        );
        
        // 添加方向类
        document.body.classList.toggle('landscape', deviceDetection.isLandscape());
        document.body.classList.toggle('portrait', !deviceDetection.isLandscape());
      }, 100);
    };

    window.addEventListener('orientationchange', handleChange);
    window.addEventListener('resize', handleChange);
    
    // 初始设置
    handleChange();
  },

  // 修复iOS Safari的视口高度问题
  fixIOSViewport: () => {
    if (!deviceDetection.isIOS()) return;

    const setVH = () => {
      const vh = window.innerHeight * 0.01;
      document.documentElement.style.setProperty('--vh', `${vh}px`);
    };

    setVH();
    window.addEventListener('resize', setVH);
    window.addEventListener('orientationchange', () => {
      setTimeout(setVH, 500);
    });
  }
};

/**
 * 触摸交互优化
 */
export const touchInteraction = {
  // 添加触摸反馈
  addTouchFeedback: (selector) => {
    const elements = document.querySelectorAll(selector);
    
    elements.forEach(element => {
      element.addEventListener('touchstart', (e) => {
        element.classList.add('touch-active');
      }, { passive: true });
      
      element.addEventListener('touchend', (e) => {
        setTimeout(() => {
          element.classList.remove('touch-active');
        }, 150);
      }, { passive: true });
      
      element.addEventListener('touchcancel', (e) => {
        element.classList.remove('touch-active');
      }, { passive: true });
    });
  },

  // 防止双击缩放
  preventDoubleTapZoom: () => {
    let lastTouchEnd = 0;
    
    document.addEventListener('touchend', (e) => {
      const now = new Date().getTime();
      if (now - lastTouchEnd <= 300) {
        e.preventDefault();
      }
      lastTouchEnd = now;
    }, false);
  },

  // 处理长按事件
  addLongPressHandler: (selector, callback, duration = 500) => {
    const elements = document.querySelectorAll(selector);
    
    elements.forEach(element => {
      let pressTimer = null;
      
      const startPress = (e) => {
        pressTimer = setTimeout(() => {
          callback(e, element);
        }, duration);
      };
      
      const cancelPress = () => {
        if (pressTimer) {
          clearTimeout(pressTimer);
          pressTimer = null;
        }
      };
      
      element.addEventListener('touchstart', startPress, { passive: true });
      element.addEventListener('touchend', cancelPress, { passive: true });
      element.addEventListener('touchmove', cancelPress, { passive: true });
      element.addEventListener('touchcancel', cancelPress, { passive: true });
    });
  }
};

/**
 * 性能优化
 */
export const performanceOptimization = {
  // 节流函数
  throttle: (func, delay) => {
    let timeoutId;
    let lastExecTime = 0;
    
    return function (...args) {
      const currentTime = Date.now();
      
      if (currentTime - lastExecTime > delay) {
        func.apply(this, args);
        lastExecTime = currentTime;
      } else {
        clearTimeout(timeoutId);
        timeoutId = setTimeout(() => {
          func.apply(this, args);
          lastExecTime = Date.now();
        }, delay - (currentTime - lastExecTime));
      }
    };
  },

  // 防抖函数
  debounce: (func, delay) => {
    let timeoutId;
    
    return function (...args) {
      clearTimeout(timeoutId);
      timeoutId = setTimeout(() => func.apply(this, args), delay);
    };
  },

  // 优化滚动性能
  optimizeScroll: () => {
    let ticking = false;
    
    const updateScrollPosition = () => {
      // 这里可以添加滚动相关的逻辑
      ticking = false;
    };
    
    const onScroll = () => {
      if (!ticking) {
        requestAnimationFrame(updateScrollPosition);
        ticking = true;
      }
    };
    
    window.addEventListener('scroll', onScroll, { passive: true });
  },

  // 延迟加载
  lazyLoad: (selector, callback) => {
    if ('IntersectionObserver' in window) {
      const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
          if (entry.isIntersecting) {
            callback(entry.target);
            observer.unobserve(entry.target);
          }
        });
      }, {
        rootMargin: '50px'
      });

      document.querySelectorAll(selector).forEach(el => {
        observer.observe(el);
      });
    } else {
      // 降级处理
      document.querySelectorAll(selector).forEach(callback);
    }
  }
};

/**
 * 调试工具
 */
export const debugTools = {
  // 显示设备信息
  showDeviceInfo: () => {
    if (process.env.NODE_ENV !== 'development') return;
    
    const info = {
      userAgent: navigator.userAgent,
      screenSize: `${window.screen.width}x${window.screen.height}`,
      viewportSize: `${window.innerWidth}x${window.innerHeight}`,
      devicePixelRatio: window.devicePixelRatio,
      isMobile: deviceDetection.isMobile(),
      isTouchDevice: deviceDetection.isTouchDevice(),
      isLandscape: deviceDetection.isLandscape(),
      screenType: deviceDetection.getScreenSize()
    };
    
    console.table(info);
  },

  // 显示性能信息
  showPerformanceInfo: () => {
    if (process.env.NODE_ENV !== 'development') return;
    
    if ('performance' in window) {
      const perfData = performance.getEntriesByType('navigation')[0];
      console.log('Page Load Performance:', {
        DOMContentLoaded: perfData.domContentLoadedEventEnd - perfData.navigationStart,
        LoadComplete: perfData.loadEventEnd - perfData.navigationStart,
        FirstPaint: performance.getEntriesByType('paint')[0]?.startTime || 'N/A'
      });
    }
  }
};

/**
 * 主初始化函数
 */
export const initMobileOptimizations = () => {
  // 设置视口
  viewportManager.setViewport();
  
  // 处理方向变化
  viewportManager.handleOrientationChange();
  
  // 修复iOS视口问题
  viewportManager.fixIOSViewport();
  
  // 初始化工具提示
  mobileTooltips.init();
  
  // 添加触摸反馈
  touchInteraction.addTouchFeedback('.header-action-btn, .parameter-card, .edit-button-compact');
  
  // 防止双击缩放
  if (deviceDetection.isMobile()) {
    touchInteraction.preventDoubleTapZoom();
  }
  
  // 优化滚动
  performanceOptimization.optimizeScroll();
  
  // 添加设备类名
  document.body.classList.add(
    deviceDetection.isMobile() ? 'mobile' : 'desktop',
    deviceDetection.isTouchDevice() ? 'touch' : 'no-touch',
    deviceDetection.getScreenSize(),
    deviceDetection.isLandscape() ? 'landscape' : 'portrait'
  );
  
  // 开发环境调试信息
  if (process.env.NODE_ENV === 'development') {
    debugTools.showDeviceInfo();
    debugTools.showPerformanceInfo();
  }
  
  console.log('Mobile optimizations initialized');
};

// CSS类的定义（添加到CSS中）
export const mobileCSS = `
.touch-active {
  transform: scale(0.95) !important;
  transition: transform 0.1s ease !important;
}

.mobile .parameter-card:hover {
  transform: none !important;
}

.mobile .header-action-btn:hover {
  transform: none !important;
}

.landscape .core-params-row {
  grid-template-columns: repeat(4, 1fr) !important;
}

.portrait.sm .core-params-row {
  grid-template-columns: 1fr !important;
}

.no-touch .tooltip {
  display: block !important;
}

.touch .tooltip:not(.mobile-visible) {
  display: none !important;
}
`;

export default {
  deviceDetection,
  mobileTooltips,
  viewportManager,
  touchInteraction,
  performanceOptimization,
  debugTools,
  initMobileOptimizations,
  mobileCSS
};