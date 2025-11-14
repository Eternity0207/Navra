import React from 'react';
import { motion } from 'framer-motion';
import { Moon, Sun, Sparkles } from 'lucide-react';
import { useTheme } from '../context/ThemeContext';

const ThemeToggle = () => {
  const { theme, toggleTheme } = useTheme();

  return (
    <motion.div 
      className="theme-toggle-wrapper"
      whileHover={{ scale: 1.05 }}
      whileTap={{ scale: 0.95 }}
    >
      <button
        onClick={toggleTheme}
        className="theme-toggle-btn"
        aria-label="Toggle theme"
      >
        <motion.div
          className="toggle-slider"
          animate={{
            x: theme === 'light' ? 0 : 24
          }}
          transition={{ type: "spring", stiffness: 300, damping: 30 }}
        >
          {theme === 'light' ? <Sun size={16} /> : <Moon size={16} />}
        </motion.div>
        <div className="toggle-track">
          <span className="toggle-icon left"><Sun size={14} /></span>
          <span className="toggle-icon right"><Moon size={14} /></span>
        </div>
      </button>
    </motion.div>
  );
};

export default ThemeToggle;
